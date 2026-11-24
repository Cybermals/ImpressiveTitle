#pragma once

/**
 * Cryptography.h
 * 
 * Shared cryptography functions.
 */

#include <vector>

#include "RakPeerInterface.h"
#include "psa/crypto.h"

using namespace std;
using namespace RakNet;


// Structures
// ==========
struct CryptoSession
{
    psa_key_id_t ecdhKey = PSA_KEY_ID_NULL;
    psa_key_id_t aesKey = PSA_KEY_ID_NULL;
    bool ready = false;

    ~CryptoSession()
    {
        if(ecdhKey != PSA_KEY_ID_NULL)
        {
            psa_destroy_key(ecdhKey);
        }

        if(aesKey != PSA_KEY_ID_NULL)
        {
            psa_destroy_key(aesKey);
        }
    }
};


// Functions
// =========
bool GenerateEcdhKeyPair(psa_key_id_t &privKey, vector<uint8_t> &pubKey);
bool DeriveAesGcmKey(psa_key_id_t privKey, const uint8_t *peerKey, size_t peerKeyLen, 
    psa_key_id_t &aesKey);
void SendClientHello(RakPeerInterface *peer, const AddressOrGUID host, 
    CryptoSession &session);
void HandleClientHello(RakPeerInterface *peer, Packet *packet,
    CryptoSession &session);
void HandleServerHello(Packet *packet, CryptoSession &session);
