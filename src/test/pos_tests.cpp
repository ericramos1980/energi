//
// Copyright (c) 2019 The Energi Core developers
//
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "base58.h"
#include "spork.h"
#include "utilstrencodings.h"
#include "validation.h"
#include "wallet/wallet.h"
#include "pos_kernel.h"
#include "consensus/validation.h"
#include "miner.h"

#include "test/test_energi.h"

#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(PoS_tests, TestChain100Setup)

BOOST_AUTO_TEST_CASE(PoS_transition_test)
{
#if 0
    const char* args[] = {
        "",
        "-debug=stake",
    };
    ParseParameters(ARRAYLEN(args), args);
    fDebug = true;
    fPrintToConsole = true;
#endif

    CScript scriptPubKey = CScript() <<  ToByteVector(coinbaseKey.GetPubKey()) << OP_CHECKSIG;

    CWallet wallet;
    pwalletMain = &wallet;
    pwalletMain->AddKeyPubKey(coinbaseKey, coinbaseKey.GetPubKey());
    pwalletMain->ScanForWalletTransactions(chainActive.Genesis(), true);
    pwalletMain->ReacceptWalletTransactions();
    pwalletMain->nStakeSplitThreshold = 1;

    CBitcoinAddress spork_address;
    spork_address.Set(coinbaseKey.GetPubKey().GetID());
    BOOST_CHECK(spork_address.IsValid());
    
    BOOST_CHECK(sporkManager.SetSporkAddress(spork_address.ToString()));
    BOOST_CHECK(sporkManager.SetPrivKey(CBitcoinSecret(coinbaseKey).ToString()));
    BOOST_CHECK(sporkManager.UpdateSpork(SPORK_15_FIRST_POS_BLOCK, 103, *connman));
    BOOST_CHECK_EQUAL(nFirstPoSBlock, 103U);
    //int last_pow_height;

    const int64_t BLOCK_TIME = chainActive.Tip()->GetBlockTimeMax() + 5;
    const int64_t BLOCK_SHIFT = pwalletMain->nHashDrift;
    int64_t time_shift = 0;
    SetMockTime(BLOCK_TIME);

    // PoW mode
    //---
    for (auto i = 2; i > 0; --i) {
        auto blk = CreateAndProcessBlock(CMutableTransactionList(), scriptPubKey);
        BOOST_CHECK(blk.IsProofOfWork());
        //last_pow_height = blk.nHeight;
        time_shift += BLOCK_SHIFT;
        SetMockTime(BLOCK_TIME + time_shift);
    }

    // PoS mode by spork
    //---
    for (auto i = 30; i > 0; --i) {
        auto blk = CreateAndProcessBlock(CMutableTransactionList(), CScript());
        BOOST_CHECK(blk.IsProofOfStake());
        BOOST_CHECK(blk.HasStake());
        time_shift += BLOCK_SHIFT;
        SetMockTime(BLOCK_TIME + time_shift);
    }

    // Still, it must continue PoS even after Spork change
    //---
    BOOST_CHECK(sporkManager.UpdateSpork(SPORK_15_FIRST_POS_BLOCK, 999999ULL, *connman));
    BOOST_CHECK_EQUAL(nFirstPoSBlock, 103U);
    
    {
        auto blk = CreateAndProcessBlock(CMutableTransactionList(), CScript());
        BOOST_CHECK(blk.IsProofOfStake());
        BOOST_CHECK(blk.HasStake());
        time_shift += BLOCK_SHIFT;
        SetMockTime(BLOCK_TIME + time_shift);
    }

    // Check signature
    //---
    {
        auto pblk = BlockAssembler(Params()).CreateNewBlock(CScript(), pwalletMain)->block;
        auto &blk = *pblk;
        
        CValidationState state;
        BOOST_CHECK(TestBlockValidity(state, Params(), blk, chainActive.Tip(), true, false));

        CKey key;
        key.MakeNewKey(true);
        BOOST_CHECK(key.SignCompact(blk.GetHash(), blk.posBlockSig));
        BOOST_CHECK(!CheckProofOfStake(state, blk));
        state = CValidationState();
        BOOST_CHECK(!TestBlockValidity(state, Params(), blk, chainActive.Tip(), true, false));

        time_shift += BLOCK_SHIFT;
        SetMockTime(BLOCK_TIME + time_shift);
    }
    
    // Check Stake
    //---
    {
        auto pblk = BlockAssembler(Params()).CreateNewBlock(CScript(), pwalletMain)->block;
        auto &blk = *pblk;

        CValidationState state;
        BOOST_CHECK(CheckProofOfStake(state, blk));
        BOOST_CHECK(TestBlockValidity(state, Params(), blk, chainActive.Tip(), true, false));

        blk.vtx.erase(blk.vtx.begin() + 1);

        BOOST_CHECK(CheckProofOfStake(state, blk)); // Yes, it's TRUE
        BOOST_CHECK(!TestBlockValidity(state, Params(), blk, chainActive.Tip(), true, false));

        time_shift += BLOCK_SHIFT;
        SetMockTime(BLOCK_TIME + time_shift);
    }
    
    // Check CoinBase
    //---
    {
        auto pblk = BlockAssembler(Params()).CreateNewBlock(CScript(), pwalletMain)->block;
        auto &blk = *pblk;

        CValidationState state;
        BOOST_CHECK(CheckProofOfStake(state, blk));
        BOOST_CHECK(TestBlockValidity(state, Params(), blk, chainActive.Tip(), true, false));

        CMutableTransaction cb{*(blk.CoinBase())};
        cb.vout[0].scriptPubKey = cb.vout[1].scriptPubKey;
        blk.CoinBase() = MakeTransactionRef(std::move(cb));

        BOOST_CHECK(CheckProofOfStake(state, blk)); // Yes, it's TRUE
        BOOST_CHECK(!TestBlockValidity(state, Params(), blk, chainActive.Tip(), true, false));

        time_shift += BLOCK_SHIFT;
        SetMockTime(BLOCK_TIME + time_shift);
    }
    
    // end
    pwalletMain = nullptr;
    nFirstPoSBlock = 999999;
}

BOOST_AUTO_TEST_SUITE_END()
