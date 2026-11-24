/**
 * Cryptography.cpp
 * 
 * Shared cryptography functions.
 */

#include <cstdint>
#include <cstring>

#include "BitStream.h"

#include "Cryptography.h"
#include "MagixNetworkDefines.h"


// Functions
// =========
bool GenerateEcdhKeyPair(psa_key_id_t &privKey, vector<uint8_t> &pubKey)
{
    // Set key attribs
    psa_key_attributes_t attribs = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(&attribs, PSA_KEY_USAGE_DERIVE);
    psa_set_key_algorithm(&attribs, PSA_ALG_ECDH);
    psa_set_key_type(&attribs, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_bits(&attribs, 256);

    // Generate key pair
    psa_status_t status = psa_generate_key(&attribs, &privKey);

    if(status != PSA_SUCCESS)
    {
        return false;
    }

    // Export public key
    pubKey.resize(PSA_EXPORT_PUBLIC_KEY_MAX_SIZE);
    size_t pubKeyLen = 0;
    status = psa_export_public_key(privKey, pubKey.data(), pubKey.size(), &pubKeyLen);

    if(status != PSA_SUCCESS)
    {
        return false;
    }

    pubKey.resize(pubKeyLen);
    return true;
}


bool DeriveAesGcmKey(psa_key_id_t privKey, const uint8_t *peerKey, size_t peerKeyLen, 
    psa_key_id_t &aesKey)
{
    // Do ECDH agreement
    vector<uint8_t> secret(64);
    size_t secretLen = 0;
    psa_status_t status = psa_raw_key_agreement(
        PSA_ALG_ECDH,
        privKey,
        peerKey,
        peerKeyLen,
        secret.data(),
        secret.size(),
        &secretLen
    );

    if(status != PSA_SUCCESS)
    {
        return false;
    }

    // Initialize session key derivation operation
    psa_key_derivation_operation_t op = PSA_KEY_DERIVATION_OPERATION_INIT;
    status = psa_key_derivation_setup(&op, PSA_ALG_HKDF(PSA_ALG_SHA_256));

    if(status != PSA_SUCCESS)
    {
        return false;
    }

    const uint8_t salt[] = "ImpressiveTitle-Handshake-Salt";
    const uint8_t info[] = "ImpressiveTitle-AES-GCM-Session";
    status = psa_key_derivation_input_bytes(
        &op,
        PSA_KEY_DERIVATION_INPUT_SALT,
        salt,
        sizeof(salt) - 1
    );

    if(status == PSA_SUCCESS)
    {
        status = psa_key_derivation_input_bytes(
            &op,
            PSA_KEY_DERIVATION_INPUT_SECRET,
            secret.data(),
            secretLen
        );
    }

    if(status == PSA_SUCCESS)
    {
        status = psa_key_derivation_input_bytes(
            &op,
            PSA_KEY_DERIVATION_INPUT_INFO,
            info,
            sizeof(info) - 1
        );
    }

    // Erase secret from memory
    memset(secret.data(), 0, secret.size());

    if(status != PSA_SUCCESS)
    {
        psa_key_derivation_abort(&op);
        return false;
    }

    // Derive session key
    psa_key_attributes_t attribs = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(
        &attribs,
        PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT
    );
    psa_set_key_algorithm(&attribs, PSA_ALG_GCM);
    psa_set_key_type(&attribs, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attribs, 128);

    status = psa_key_derivation_output_key(&attribs, &op, &aesKey);
    psa_key_derivation_abort(&op);
    return (status == PSA_SUCCESS);
}


void SendClientHello(RakPeerInterface *peer, const AddressOrGUID host, 
    CryptoSession &session)
{
    // Generate ECDH key pair
    vector<uint8_t> pubKey;

    if(!GenerateEcdhKeyPair(session.ecdhKey, pubKey))
    {
        return;
    }

    // Send public key
    BitStream bs;
    bs.Write(MessageID(ID_CRYPTO_CLIENT_HELLO));
    bs.Write(pubKey.size());
    bs.WriteAlignedBytes(pubKey.data(), pubKey.size());
    peer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, host, false);
}


void HandleClientHello(RakPeerInterface *peer, Packet *packet,
    CryptoSession &session)
{
    // Read peer key
    BitStream bs(packet->data, packet->length, false);
    MessageID msgId = 0;
    int peerKeyLen = 0;
    vector<uint8_t> peerKey;

    bs.Read(msgId);

    if(!bs.Read(peerKeyLen) || peerKeyLen == 0 || peerKeyLen > 256)
    {
        return;
    }

    peerKey.resize(peerKeyLen);

    if(!bs.ReadAlignedBytes(peerKey.data(), peerKey.size()))
    {
        return;
    }

    // Generate ECDH key pair
    vector<uint8_t> pubKey;

    if(!GenerateEcdhKeyPair(session.ecdhKey, pubKey))
    {
        return;
    }

    // Derive AES-GCM key
    if(!DeriveAesGcmKey(session.ecdhKey, peerKey.data(), peerKey.size(), 
        session.aesKey))
    {
        return;
    }

    session.ready = true;

    // Send reply
    bs.Reset();
    bs.Write(MessageID(ID_CRYPTO_SERVER_HELLO));
    bs.Write(pubKey.size());
    bs.WriteAlignedBytes(pubKey.data(), pubKey.size());
    peer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress,
        false);
}


void HandleServerHello(Packet *packet, CryptoSession &session)
{
    // Read peer key
    BitStream bs(packet->data, packet->length, false);
    MessageID msgId = 0;
    int peerKeyLen = 0;
    vector<uint8_t> peerKey;

    bs.Read(msgId);

    if(!bs.Read(peerKeyLen) || peerKeyLen == 0 || peerKeyLen > 256)
    {
        return;
    }

    peerKey.resize(peerKeyLen);

    if(!bs.ReadAlignedBytes(peerKey.data(), peerKey.size()))
    {
        return;
    }

    // Derive AES-GCM key
    if(DeriveAesGcmKey(session.ecdhKey, peerKey.data(), peerKey.size(), 
        session.aesKey))
    {
        session.ready = true;
    }
}
