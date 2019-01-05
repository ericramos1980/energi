// Copyright (c) 2017-2019 The Energi Core developers
// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "primitives/block.h"

#include "hash.h"
#include "script/standard.h"
#include "script/sign.h"
#include "tinyformat.h"
#include "utilstrencodings.h"
#include "crypto/common.h"
#include "compat/endian.h"
#include "dag_singleton.h"
#include "keystore.h"
#include "pos_kernel.h"
#include "util.h"

#include <algorithm>

namespace
{
    #pragma pack(push, 1)
    /**
    *   The Keccak-256 hash of this structure is used as input for egihash
    *   It is a truncated block header with a deterministic encoding
    *   All integer values are little endian
    *   Hashes are the nul-terminated hex encoded representation as if ToString() was called
    */
    struct CBlockHeaderTruncatedLE
    {
        int32_t nVersion;
        char hashPrevBlock[65];
        char hashMerkleRoot[65];
        uint32_t nTime;
        uint32_t nBits;
        uint32_t nHeight;

        CBlockHeaderTruncatedLE(CBlockHeader const & h)
        : nVersion(htole32(h.nVersion))
        , hashPrevBlock{0}
        , hashMerkleRoot{0}
        , nTime(htole32(h.nTime))
        , nBits(htole32(h.nBits))
        , nHeight(htole32(h.nHeight))
        {
            auto prevHash = h.hashPrevBlock.ToString();
            memcpy(hashPrevBlock, prevHash.c_str(), (std::min)(prevHash.size(), sizeof(hashPrevBlock)));

            auto merkleRoot = h.hashMerkleRoot.ToString();
            memcpy(hashMerkleRoot, merkleRoot.c_str(), (std::min)(merkleRoot.size(), sizeof(hashMerkleRoot)));
        }
    };
    static_assert(sizeof(CBlockHeaderTruncatedLE) == 146, "CBlockHeaderTruncatedLE has incorrect size");

    struct CBlockHeaderFullLE : public CBlockHeaderTruncatedLE
    {
        uint64_t nNonce;
        char hashMix[65];

        CBlockHeaderFullLE(CBlockHeader const & h)
        : CBlockHeaderTruncatedLE(h)
        , nNonce(h.nNonce)
        , hashMix{0}
        {
            auto mixString = h.hashMix.ToString();
            memcpy(hashMix, mixString.c_str(), (std::min)(mixString.size(), sizeof(hashMix)));
        }
    };
    static_assert(sizeof(CBlockHeaderFullLE) == 219, "CBlockHeaderFullLE has incorrect size");
    #pragma pack(pop)
}

uint256 CBlockHeader::GetPOWHash()
{
    if (IsProofOfStake()) {
        return hashProofOfStake();
    }
    
    CBlockHeaderTruncatedLE truncatedBlockHeader(*this);
    egihash::h256_t headerHash(&truncatedBlockHeader, sizeof(truncatedBlockHeader));
    egihash::result_t ret;
    // if we have a DAG loaded, use it
    auto const & dag = ActiveDAG();
    if (dag && ((nHeight / egihash::constants::EPOCH_LENGTH) == dag->epoch()))
    {
        ret = egihash::full::hash(*dag, headerHash, nNonce);
    }
    else // otherwise all we can do is generate a light hash
    {
        // TODO: pre-load caches and seed hashes
        ret = egihash::light::hash(egihash::cache_t(nHeight), headerHash, nNonce);
    }

    hashMix = uint256(ret.mixhash);
    return uint256(ret.value);
}

uint256 CBlockHeader::GetPOWHash() const
{
    if (IsProofOfStake()) {
        return hashProofOfStake();
    }
    
    CBlockHeaderTruncatedLE truncatedBlockHeader(*this);
    egihash::h256_t headerHash(&truncatedBlockHeader, sizeof(truncatedBlockHeader));
    egihash::result_t ret;
    // if we have a DAG loaded, use it
    auto const & dag = ActiveDAG();
    if (dag && ((nHeight / egihash::constants::EPOCH_LENGTH) == dag->epoch()))
    {
        ret = egihash::full::hash(*dag, headerHash, nNonce);
    }
    else // otherwise all we can do is generate a light hash
    {
        // TODO: pre-load caches and seed hashes
        ret = egihash::light::hash(egihash::cache_t(nHeight), headerHash, nNonce);
    }

    return uint256(ret.value);
}

uint256 CBlockHeader::GetHash() const
{
    if (IsProofOfStake()) {
        CDataStream sh(SER_GETHASH, 0);
        
        sh << *this;

        egihash::h256_t blockHash(sh.data(), sh.size());
        return uint256(blockHash);
    }

    // return a Keccak-256 hash of the full block header, including nonce and mixhash
    CBlockHeaderFullLE fullBlockHeader(*this);
    egihash::h256_t blockHash(&fullBlockHeader, sizeof(fullBlockHeader));
    return uint256(blockHash);
}

std::string CBlock::ToString() const
{
    std::stringstream s;
    
    if (IsProofOfStake()) {
        s << strprintf("CBlock(hash=%s, ver=0x%08x, hashPrevBlock=%s, hashMerkleRoot=%s, nTime=%u, nBits=%08x, nHeight=%u, hashMix=%s, nNonce=%u, posStakeHash=%s, posStakeN=%u, posPubKey=%u, posBlockSig=%u vtx=%u)\n",
            GetHash().ToString(),
            nVersion,
            hashPrevBlock.ToString(),
            hashMerkleRoot.ToString(),
            nTime, nBits, nHeight,
            hashMix.ToString(),
            nNonce,
            posStakeHash.ToString(),
            posStakeN,
            posPubKey.size(),
            posBlockSig.size(),
            vtx.size());
    } else {
        s << strprintf("CBlock(hash=%s, hashPow=%s, ver=0x%08x, hashPrevBlock=%s, hashMerkleRoot=%s, nTime=%u, nBits=%08x, nHeight=%u, hashMix=%s, nNonce=%u, vtx=%u)\n",
                GetHash().ToString(),
                GetPOWHash().ToString(),
                nVersion,
                hashPrevBlock.ToString(),
                hashMerkleRoot.ToString(),
                nTime, nBits, nHeight,
                hashMix.ToString(),
                nNonce,
                vtx.size());
    }

    for (unsigned int i = 0; i < vtx.size(); i++)
    {
        s << "  " << vtx[i]->ToString() << "\n";
    }
    return s.str();
}


// ppcoin: sign block
bool CBlockHeader::SignBlock(const CKeyStore& keystore)
{
    if (IsProofOfStake())
    {
        if (!posPubKey.IsValid())
            return false;

        CKey key;

        if (!keystore.GetKey(posPubKey.GetID(), key)) {
            return false;
        }

        if (!key.SignCompact(GetHash(), posBlockSig)) {
            return false;
        }

        return true;
    }

    return true;
}

bool CBlockHeader::CheckBlockSignature(const CKeyID& key_id) const
{
    if (!IsProofOfStake()) {
        return true;
    }
    
    if (posBlockSig.empty()) {
        return false;
    }
    
    auto hash = GetHash();
    posPubKey.RecoverCompact(hash, posBlockSig);

    if (!posPubKey.IsValid()) {
        return false;
    }

    return posPubKey.GetID() == key_id;
}

const CPubKey& CBlockHeader::BlockPubKey() const
{
    // In case it's read from disk
    if (!posPubKey.IsValid() && !posBlockSig.empty()) {
        posPubKey.RecoverCompact(GetHash(), posBlockSig);
    }

    return posPubKey;
}

bool CBlock::HasCoinBase() const {
    return (!vtx.empty() && CoinBase()->IsCoinBase());
}

bool CBlock::HasStake() const {
    if (!IsProofOfStake() || (vtx.size() < 2) || posBlockSig.empty()) {
        return false;
    }

    BlockPubKey();

    if (!posPubKey.IsValid()) {
        return false;
    }

    const auto spk = GetScriptForDestination(posPubKey.GetID());
    const auto& cb_vout = CoinBase()->vout;
    const auto& stake = Stake();

    if (cb_vout.empty() || stake->vin.empty() || stake->vout.empty()) {
        return false;
    }

    // Check it's the same stake
    if (stake->vin[0].prevout != COutPoint(posStakeHash, posStakeN)) {
        return false;
    }

    // Check primary coinbase output
    if (cb_vout[0].scriptPubKey != spk) {
        return false;
    }

    // Check stake outputs
    CAmount total_amt = 0;

    for (const auto &so : stake->vout) {
        if (so.scriptPubKey != spk) {
            return false;
        }

        total_amt += so.nValue;
    }

    if (total_amt < MIN_STAKE_AMOUNT) {
        return false;
    }

    return true;
}
