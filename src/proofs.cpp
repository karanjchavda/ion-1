// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2015-2017 The Silk Network developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'

#include "proofs.h"

#include "chainparams.h"
#include "main.h"
#include "uint256.h"
#include "util.h"
#include "amount.h"
#include "checkpoints.h"

#include <math.h>
#include <stdint.h> 

unsigned int nStakeMinAge       = 8 * 60 * 60;
unsigned int nModifierInterval  = 2 * 60 * 60;

MapCheckpoints mapPremineSendBlocks =
	boost::assign::map_list_of
	(1,	("0x0000004cf5ffbf2e31a9aa07c86298efb01a30b8911b80af7473d1114715084b") ), // Premine
	(10,	("0x00000032f5a96d31d74b380c0336445baccb73a01bdbedec868283019bad7016") ),  // Confirmation of Premine
	(22,	("0x00000002e04f91402d78b84433ec744aacac5c40952b918fe09a7d623ac33967") ),
	(32,	("0x0000001880da8fd09cc6f5e93315135fe686eb49f9054c807fa810d56ebb013b") ),
	(35,	("0x0000000af6204fd43bb9cafea1dd192c245979d4dd7bde19efb92f633589ade5") ),
	(45,	("0x00000006d6b9e9fba4dee10bc63ca7ea764c80c2b9c4fa6ddedb944eb288a371") )
;

uint256 CBlock::GetHash() const {
	return GetPoWHash();
}

uint256 CBlock::GetPoWHash() const {
	return Hash(BEGIN(nVersion), END(nNonce));
}

int DetermineCoinbaseMaturity() {
	if(pindexBest->nHeight <= 100) {
		return (int)10; // This will allow for premine distribution to propogate faster
	} else {
		return (int)60; // Coinbase will take approx. 1 hr to reach confirmation
	}
}

// miner's coin base reward
int64_t GetCoinbaseValue(int nHeight, CAmount nFees)
{
    CAmount nSubsidy = 0;

	if(nHeight == 1) {
		nSubsidy = 16400000 * COIN;
	}

    return nSubsidy;
}

// miner's coin stake reward based on coin age spent (coin-days)
int64_t GetCoinstakeValue(int64_t nCoinAge, CAmount nFees, int nHeight)
{
	CAmount nSubsidy = 0.2 * COIN;

	if(nHeight <= 125146) {
		nSubsidy = 23 * COIN;
	} else if(nHeight <= 568622) {
		nSubsidy = 17 * COIN;
	} else if(nHeight <= 1012098) {
		nSubsidy = 11.5 * COIN;
	} else if(nHeight <= 1455574) {
		nSubsidy = 5.75 * COIN;
	} else if(nHeight <= 3675950) {
		nSubsidy = 1.85 * COIN;
	} else {
		nSubsidy = 0.2 * COIN;
	}

    return nSubsidy + nFees;
}

unsigned int GetNextTargetRequired(const CBlockIndex* pindexLast, bool fProofOfStake)
{
    uint256 bnTargetLimit = fProofOfStake ? Params().ProofOfStakeLimit() : Params().ProofOfWorkLimit();

    if (pindexLast == NULL)
        return bnTargetLimit.GetCompact(); // genesis block

    const CBlockIndex* pindexPrev = GetLastBlockIndex(pindexLast, fProofOfStake);
    if (pindexPrev->pprev == NULL)
        return bnTargetLimit.GetCompact(); // first block
    const CBlockIndex* pindexPrevPrev = GetLastBlockIndex(pindexPrev->pprev, fProofOfStake);
    if (pindexPrevPrev->pprev == NULL)
        return bnTargetLimit.GetCompact(); // second block

    int64_t nActualSpacing = pindexPrev->GetBlockTime() - pindexPrevPrev->GetBlockTime();

    if (nActualSpacing < 0) {
        nActualSpacing = nTargetSpacing;
    }
    else if (fProofOfStake && nActualSpacing > nTargetSpacing * 10) {
         nActualSpacing = nTargetSpacing * 10;
    }

    // target change every block
    // retarget with exponential moving toward target spacing
    // Includes fix for wrong retargeting difficulty by Mammix2
    uint256 bnNew;
    bnNew.SetCompact(pindexPrev->nBits);

    int64_t nInterval = fProofOfStake ? 10 : 10;
    bnNew *= ((nInterval - 1) * nTargetSpacing + nActualSpacing + nActualSpacing);
    bnNew /= ((nInterval + 1) * nTargetSpacing);

    if (bnNew <= 0 || bnNew > bnTargetLimit)
        bnNew = bnTargetLimit;

    return bnNew.GetCompact();
}

bool CheckProofOfWork(uint256 hash, unsigned int nBits)
{
    uint256 bnTarget;
    bnTarget.SetCompact(nBits);

    // Check range
    if (bnTarget <= 0 || bnTarget > Params().ProofOfWorkLimit())
        return error("CheckProofOfWork() : nBits below minimum work");

    // Check proof of work matches claimed amount
    if (hash > bnTarget)
        return error("CheckProofOfWork() : hash doesn't match nBits");

    return true;
}