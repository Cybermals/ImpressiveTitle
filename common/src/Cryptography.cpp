/**
 * Cryptography.cpp
 * 
 * Shared cryptography functions.
 */

#include <cstdint>
#include <cstring>
#include <iostream>

#include "BitStream.h"

#include "Cryptography.h"
#include "MagixNetworkDefines.h"


// Functions
// =========
bool GenerateEcdhKeyPair(psa_key_id_t &privKey, std::vector<uint8_t> &pubKey)
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
    // Initialize key derivation operation
    psa_key_derivation_operation_t op = PSA_KEY_DERIVATION_OPERATION_INIT;
    psa_status_t status = psa_key_derivation_setup(
        &op, PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)));

    if(status != PSA_SUCCESS)
    {
        return false;
    }

    // Set salt
    const uint8_t salt[] = "ImpressiveTitle-Handshake-Salt";
    const uint8_t info[] = "ImpressiveTitle-AES-GCM-Session";
    status = psa_key_derivation_input_bytes(
        &op,
        PSA_KEY_DERIVATION_INPUT_SALT,
        salt,
        sizeof(salt) - 1
    );

    // Do key derivation agreement
    if(status == PSA_SUCCESS)
    {
        status = psa_key_derivation_key_agreement(
            &op,
            PSA_KEY_DERIVATION_INPUT_SECRET,
            privKey,
            peerKey,
            peerKeyLen
        );
    }

    // Set info
    if(status == PSA_SUCCESS)
    {
        status = psa_key_derivation_input_bytes(
            &op,
            PSA_KEY_DERIVATION_INPUT_INFO,
            info,
            sizeof(info) - 1
        );
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
    std::vector<uint8_t> pubKey;

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
    size_t peerKeyLen = 0;
    std::vector<uint8_t> peerKey;

    bs.Read(msgId);

    if(!bs.Read(peerKeyLen) || peerKeyLen == 0 || peerKeyLen > 256)
    {
        std::cout << "Failed to read peer key length." << std::endl;
        return;
    }

    peerKey.resize(peerKeyLen);

    if(!bs.ReadAlignedBytes(peerKey.data(), peerKey.size()))
    {
        std::cout << "Failed to read peer key." << std::endl;
        return;
    }

    // Generate ECDH key pair
    std::vector<uint8_t> pubKey;

    if(!GenerateEcdhKeyPair(session.ecdhKey, pubKey))
    {
        std::cout << "Failed to generate ECDH key pair." << std::endl;
        return;
    }

    // Derive AES-GCM key
    if(!DeriveAesGcmKey(session.ecdhKey, peerKey.data(), peerKey.size(), 
        session.aesKey))
    {
        std::cout << "Failed to derive AES-GCM key." << std::endl;
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
    std::cout << "Send server hello." << std::endl;
}


void HandleServerHello(Packet *packet, CryptoSession &session)
{
    // Read peer key
    BitStream bs(packet->data, packet->length, false);
    MessageID msgId = 0;
    size_t peerKeyLen = 0;
    std::vector<uint8_t> peerKey;

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

bool EncryptBitStream(CryptoSession &session, const BitStream &in, BitStream &out)
{
    // Validate inputs
    if(!session.ready || session.aesKey == PSA_KEY_ID_NULL)
    {
        return false;
    }

    // Read message ID
    BitStream bs(in.GetData(), in.GetNumberOfBytesUsed(), true);
    const size_t bufLen = in.GetNumberOfBytesUsed();
    std::vector<uint8_t> buf(bufLen);

    if(bufLen)
    {
        bs.ReadAlignedBytes(buf.data(), bufLen);
    }

    // Generate random IV
    std::vector<uint8_t> iv(GCM_IV_SIZE);

    if(psa_generate_random(iv.data(), iv.size()) != PSA_SUCCESS)
    {
        return false;
    }

    // Encrypt data
    std::vector<uint8_t> sealed(bufLen + GCM_TAG_SIZE);
    size_t sealedLen = 0;
    psa_status_t status = psa_aead_encrypt(
        session.aesKey,
        PSA_ALG_GCM,
        iv.data(),
        iv.size(),
        nullptr,
        0,
        buf.data(),
        buf.size(),
        sealed.data(),
        sealed.size(),
        &sealedLen
    );

    // Erase unencrypted data from memory
    memset(buf.data(), 0, buf.size());

    if(status != PSA_SUCCESS)
    {
        return false;
    }

    // Write encrypted data
    sealed.resize(sealedLen);

    out.Reset();
    out.Write(MessageID(ID_CRYPTO_ENCRYPTED_MSG));
    out.Write(iv.size());
    out.WriteAlignedBytes(iv.data(), iv.size());
    out.Write(sealed.size());
    out.WriteAlignedBytes(sealed.data(), sealed.size());
    return true;
}


bool DecryptPacket(CryptoSession &session, Packet *packet)
{
    // Validate inputs
    if(!session.ready || session.aesKey == PSA_KEY_ID_NULL || !packet)
    {
        return false;
    }

    // Read message ID
    BitStream bs(packet->data, packet->length, false);
    MessageID msgId = 0;

    if(!bs.Read(msgId) || msgId != ID_CRYPTO_ENCRYPTED_MSG)
    {
        return false;
    }

    // Read IV
    size_t ivLen = 0;

    if(!bs.Read(ivLen) || ivLen != GCM_IV_SIZE)
    {
        return false;
    }

    std::vector<uint8_t> iv(ivLen);

    if(!bs.ReadAlignedBytes(iv.data(), iv.size()))
    {
        return false;
    }

    // Read encrypted data
    size_t sealedLen = 0;

    if(!bs.Read(sealedLen) || sealedLen < GCM_TAG_SIZE)
    {
        return false;
    }

    const size_t remainingBytes = bs.GetNumberOfUnreadBits() / 8;

    if(sealedLen > remainingBytes)
    {
        return false;
    }

    std::vector<uint8_t> sealed(sealedLen);

    if(!bs.ReadAlignedBytes(sealed.data(), sealed.size()))
    {
        return false;
    }

    // Decrypt data
    std::vector<uint8_t> buf(sealed.size() - GCM_TAG_SIZE + 1);
    size_t bufLen = 0;
    psa_status_t status = psa_aead_decrypt(
        session.aesKey,
        PSA_ALG_GCM,
        iv.data(),
        iv.size(),
        nullptr,
        0,
        sealed.data(),
        sealed.size(),
        buf.data(),
        buf.size(),
        &bufLen
    );

    if(status != PSA_SUCCESS)
    {
        return false;
    }

    // Write decrypted data back to the original packet. Since the decrypted 
    // data is always smaller than the original encrypted data, we don't need
    // to resize the packet data buffer.
    memcpy(packet->data, buf.data(), bufLen);
    packet->length = bufLen;
    return true;
}
