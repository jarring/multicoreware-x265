/* The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third party
 * and contributor rights, including patent rights, and no such rights are
 * granted under this license.
 *
 * Copyright (c) 2010-2013, ITU/ISO/IEC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  * Neither the name of the ITU/ISO/IEC nor the names of its contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/** \file     TEncEntropy.cpp
    \brief    entropy encoder class
*/

#include "TEncEntropy.h"
#include "TLibCommon/TypeDef.h"
#include "TLibCommon/TComSampleAdaptiveOffset.h"

//! \ingroup TLibEncoder
//! \{

Void TEncEntropy::setEntropyCoder(TEncEntropyIf* e, TComSlice* pcSlice)
{
    m_pcEntropyCoderIf = e;
    m_pcEntropyCoderIf->setSlice(pcSlice);
}

Void TEncEntropy::encodeSliceHeader(TComSlice* pcSlice)
{
    if (pcSlice->getSPS()->getUseSAO())
    {
        SAOParam *saoParam = pcSlice->getPic()->getPicSym()->getSaoParam();
        pcSlice->setSaoEnabledFlag(saoParam->bSaoFlag[0]);
        {
            pcSlice->setSaoEnabledFlagChroma(saoParam->bSaoFlag[1]);
        }
    }

    m_pcEntropyCoderIf->codeSliceHeader(pcSlice);
}

Void  TEncEntropy::encodeTilesWPPEntryPoint(TComSlice* pSlice)
{
    m_pcEntropyCoderIf->codeTilesWPPEntryPoint(pSlice);
}

Void TEncEntropy::encodeTerminatingBit(UInt uiIsLast)
{
    m_pcEntropyCoderIf->codeTerminatingBit(uiIsLast);
}

Void TEncEntropy::encodeSliceFinish()
{
    m_pcEntropyCoderIf->codeSliceFinish();
}

Void TEncEntropy::encodePPS(TComPPS* pcPPS)
{
    m_pcEntropyCoderIf->codePPS(pcPPS);
}

Void TEncEntropy::encodeSPS(TComSPS* pcSPS)
{
    m_pcEntropyCoderIf->codeSPS(pcSPS);
}

Void TEncEntropy::encodeCUTransquantBypassFlag(TComDataCU* cu, UInt absPartIdx, Bool bRD)
{
    if (bRD)
    {
        absPartIdx = 0;
    }
    m_pcEntropyCoderIf->codeCUTransquantBypassFlag(cu, absPartIdx);
}

Void TEncEntropy::encodeVPS(TComVPS* pcVPS)
{
    m_pcEntropyCoderIf->codeVPS(pcVPS);
}

Void TEncEntropy::encodeSkipFlag(TComDataCU* cu, UInt absPartIdx, Bool bRD)
{
    if (cu->getSlice()->isIntra())
    {
        return;
    }
    if (bRD)
    {
        absPartIdx = 0;
    }
    m_pcEntropyCoderIf->codeSkipFlag(cu, absPartIdx);
}

/** encode merge flag
 * \param cu
 * \param absPartIdx
 * \returns Void
 */
Void TEncEntropy::encodeMergeFlag(TComDataCU* cu, UInt absPartIdx)
{
    // at least one merge candidate exists
    m_pcEntropyCoderIf->codeMergeFlag(cu, absPartIdx);
}

/** encode merge index
 * \param cu
 * \param absPartIdx
 * \param uiPUIdx
 * \param bRD
 * \returns Void
 */
Void TEncEntropy::encodeMergeIndex(TComDataCU* cu, UInt absPartIdx, Bool bRD)
{
    if (bRD)
    {
        absPartIdx = 0;
        assert(cu->getPartitionSize(absPartIdx) == SIZE_2Nx2N);
    }
    m_pcEntropyCoderIf->codeMergeIndex(cu, absPartIdx);
}

/** encode prediction mode
 * \param cu
 * \param absPartIdx
 * \param bRD
 * \returns Void
 */
Void TEncEntropy::encodePredMode(TComDataCU* cu, UInt absPartIdx, Bool bRD)
{
    if (bRD)
    {
        absPartIdx = 0;
    }
    if (cu->getSlice()->isIntra())
    {
        return;
    }

    m_pcEntropyCoderIf->codePredMode(cu, absPartIdx);
}

// Split mode
Void TEncEntropy::encodeSplitFlag(TComDataCU* cu, UInt absPartIdx, UInt depth, Bool bRD)
{
    if (bRD)
    {
        absPartIdx = 0;
    }
    m_pcEntropyCoderIf->codeSplitFlag(cu, absPartIdx, depth);
}

/** encode partition size
 * \param cu
 * \param absPartIdx
 * \param depth
 * \param bRD
 * \returns Void
 */
Void TEncEntropy::encodePartSize(TComDataCU* cu, UInt absPartIdx, UInt depth, Bool bRD)
{
    if (bRD)
    {
        absPartIdx = 0;
    }
    m_pcEntropyCoderIf->codePartSize(cu, absPartIdx, depth);
}

/** Encode I_PCM information.
 * \param cu pointer to CU
 * \param absPartIdx CU index
 * \param bRD flag indicating estimation or encoding
 * \returns Void
 */
Void TEncEntropy::encodeIPCMInfo(TComDataCU* cu, UInt absPartIdx, Bool bRD)
{
    if (!cu->getSlice()->getSPS()->getUsePCM()
        || cu->getWidth(absPartIdx) > (1 << cu->getSlice()->getSPS()->getPCMLog2MaxSize())
        || cu->getWidth(absPartIdx) < (1 << cu->getSlice()->getSPS()->getPCMLog2MinSize()))
    {
        return;
    }

    if (bRD)
    {
        absPartIdx = 0;
    }

    m_pcEntropyCoderIf->codeIPCMInfo(cu, absPartIdx);
}

Void TEncEntropy::xEncodeTransform(TComDataCU* cu, UInt offsetLuma, UInt offsetChroma, UInt absPartIdx, UInt depth, UInt width, UInt height, UInt uiTrIdx, Bool& bCodeDQP)
{
    const UInt uiSubdiv = cu->getTransformIdx(absPartIdx) + cu->getDepth(absPartIdx) > depth;
    const UInt uiLog2TrafoSize = g_convertToBit[cu->getSlice()->getSPS()->getMaxCUWidth()] + 2 - depth;
    UInt cbfY = cu->getCbf(absPartIdx, TEXT_LUMA, uiTrIdx);
    UInt cbfU = cu->getCbf(absPartIdx, TEXT_CHROMA_U, uiTrIdx);
    UInt cbfV = cu->getCbf(absPartIdx, TEXT_CHROMA_V, uiTrIdx);

    if (uiTrIdx == 0)
    {
        m_bakAbsPartIdxCU = absPartIdx;
    }
    if (uiLog2TrafoSize == 2)
    {
        UInt partNum = cu->getPic()->getNumPartInCU() >> ((depth - 1) << 1);
        if ((absPartIdx % partNum) == 0)
        {
            m_uiBakAbsPartIdx   = absPartIdx;
            m_uiBakChromaOffset = offsetChroma;
        }
        else if ((absPartIdx % partNum) == (partNum - 1))
        {
            cbfU = cu->getCbf(m_uiBakAbsPartIdx, TEXT_CHROMA_U, uiTrIdx);
            cbfV = cu->getCbf(m_uiBakAbsPartIdx, TEXT_CHROMA_V, uiTrIdx);
        }
    }

    if (cu->getPredictionMode(absPartIdx) == MODE_INTRA && cu->getPartitionSize(absPartIdx) == SIZE_NxN && depth == cu->getDepth(absPartIdx))
    {
        assert(uiSubdiv);
    }
    else if (cu->getPredictionMode(absPartIdx) == MODE_INTER && (cu->getPartitionSize(absPartIdx) != SIZE_2Nx2N) && depth == cu->getDepth(absPartIdx) &&  (cu->getSlice()->getSPS()->getQuadtreeTUMaxDepthInter() == 1))
    {
        if (uiLog2TrafoSize > cu->getQuadtreeTULog2MinSizeInCU(absPartIdx))
        {
            assert(uiSubdiv);
        }
        else
        {
            assert(!uiSubdiv);
        }
    }
    else if (uiLog2TrafoSize > cu->getSlice()->getSPS()->getQuadtreeTULog2MaxSize())
    {
        assert(uiSubdiv);
    }
    else if (uiLog2TrafoSize == cu->getSlice()->getSPS()->getQuadtreeTULog2MinSize())
    {
        assert(!uiSubdiv);
    }
    else if (uiLog2TrafoSize == cu->getQuadtreeTULog2MinSizeInCU(absPartIdx))
    {
        assert(!uiSubdiv);
    }
    else
    {
        assert(uiLog2TrafoSize > cu->getQuadtreeTULog2MinSizeInCU(absPartIdx));
        m_pcEntropyCoderIf->codeTransformSubdivFlag(uiSubdiv, 5 - uiLog2TrafoSize);
    }

    const UInt trDepthCurr = depth - cu->getDepth(absPartIdx);
    const Bool bFirstCbfOfCU = trDepthCurr == 0;
    if (bFirstCbfOfCU || uiLog2TrafoSize > 2)
    {
        if (bFirstCbfOfCU || cu->getCbf(absPartIdx, TEXT_CHROMA_U, trDepthCurr - 1))
        {
            m_pcEntropyCoderIf->codeQtCbf(cu, absPartIdx, TEXT_CHROMA_U, trDepthCurr);
        }
        if (bFirstCbfOfCU || cu->getCbf(absPartIdx, TEXT_CHROMA_V, trDepthCurr - 1))
        {
            m_pcEntropyCoderIf->codeQtCbf(cu, absPartIdx, TEXT_CHROMA_V, trDepthCurr);
        }
    }
    else if (uiLog2TrafoSize == 2)
    {
        assert(cu->getCbf(absPartIdx, TEXT_CHROMA_U, trDepthCurr) == cu->getCbf(absPartIdx, TEXT_CHROMA_U, trDepthCurr - 1));
        assert(cu->getCbf(absPartIdx, TEXT_CHROMA_V, trDepthCurr) == cu->getCbf(absPartIdx, TEXT_CHROMA_V, trDepthCurr - 1));
    }

    if (uiSubdiv)
    {
        UInt size;
        width  >>= 1;
        height >>= 1;
        size = width * height;
        uiTrIdx++;
        ++depth;
        const UInt partNum = cu->getPic()->getNumPartInCU() >> (depth << 1);

        xEncodeTransform(cu, offsetLuma, offsetChroma, absPartIdx, depth, width, height, uiTrIdx, bCodeDQP);

        absPartIdx += partNum;
        offsetLuma += size;
        offsetChroma += (size >> 2);
        xEncodeTransform(cu, offsetLuma, offsetChroma, absPartIdx, depth, width, height, uiTrIdx, bCodeDQP);

        absPartIdx += partNum;
        offsetLuma += size;
        offsetChroma += (size >> 2);
        xEncodeTransform(cu, offsetLuma, offsetChroma, absPartIdx, depth, width, height, uiTrIdx, bCodeDQP);

        absPartIdx += partNum;
        offsetLuma += size;
        offsetChroma += (size >> 2);
        xEncodeTransform(cu, offsetLuma, offsetChroma, absPartIdx, depth, width, height, uiTrIdx, bCodeDQP);
    }
    else
    {
        {
            DTRACE_CABAC_VL(g_nSymbolCounter++);
            DTRACE_CABAC_T("\tTrIdx: abspart=");
            DTRACE_CABAC_V(absPartIdx);
            DTRACE_CABAC_T("\tdepth=");
            DTRACE_CABAC_V(depth);
            DTRACE_CABAC_T("\ttrdepth=");
            DTRACE_CABAC_V(cu->getTransformIdx(absPartIdx));
            DTRACE_CABAC_T("\n");
        }

        if (cu->getPredictionMode(absPartIdx) != MODE_INTRA && depth == cu->getDepth(absPartIdx) && !cu->getCbf(absPartIdx, TEXT_CHROMA_U, 0) && !cu->getCbf(absPartIdx, TEXT_CHROMA_V, 0))
        {
            assert(cu->getCbf(absPartIdx, TEXT_LUMA, 0));
            //      printf( "saved one bin! " );
        }
        else
        {
            m_pcEntropyCoderIf->codeQtCbf(cu, absPartIdx, TEXT_LUMA, cu->getTransformIdx(absPartIdx));
        }

        if (cbfY || cbfU || cbfV)
        {
            // dQP: only for LCU once
            if (cu->getSlice()->getPPS()->getUseDQP())
            {
                if (bCodeDQP)
                {
                    encodeQP(cu, m_bakAbsPartIdxCU);
                    bCodeDQP = false;
                }
            }
        }
        if (cbfY)
        {
            Int trWidth = width;
            Int trHeight = height;
            m_pcEntropyCoderIf->codeCoeffNxN(cu, (cu->getCoeffY() + offsetLuma), absPartIdx, trWidth, trHeight, depth, TEXT_LUMA);
        }
        if (uiLog2TrafoSize > 2)
        {
            Int trWidth = width >> 1;
            Int trHeight = height >> 1;
            if (cbfU)
            {
                m_pcEntropyCoderIf->codeCoeffNxN(cu, (cu->getCoeffCb() + offsetChroma), absPartIdx, trWidth, trHeight, depth, TEXT_CHROMA_U);
            }
            if (cbfV)
            {
                m_pcEntropyCoderIf->codeCoeffNxN(cu, (cu->getCoeffCr() + offsetChroma), absPartIdx, trWidth, trHeight, depth, TEXT_CHROMA_V);
            }
        }
        else
        {
            UInt partNum = cu->getPic()->getNumPartInCU() >> ((depth - 1) << 1);
            if ((absPartIdx % partNum) == (partNum - 1))
            {
                Int trWidth = width;
                Int trHeight = height;
                if (cbfU)
                {
                    m_pcEntropyCoderIf->codeCoeffNxN(cu, (cu->getCoeffCb() + m_uiBakChromaOffset), m_uiBakAbsPartIdx, trWidth, trHeight, depth, TEXT_CHROMA_U);
                }
                if (cbfV)
                {
                    m_pcEntropyCoderIf->codeCoeffNxN(cu, (cu->getCoeffCr() + m_uiBakChromaOffset), m_uiBakAbsPartIdx, trWidth, trHeight, depth, TEXT_CHROMA_V);
                }
            }
        }
    }
}

// Intra direction for Luma
Void TEncEntropy::encodeIntraDirModeLuma(TComDataCU* cu, UInt absPartIdx, Bool isMultiplePU)
{
    m_pcEntropyCoderIf->codeIntraDirLumaAng(cu, absPartIdx, isMultiplePU);
}

// Intra direction for Chroma
Void TEncEntropy::encodeIntraDirModeChroma(TComDataCU* cu, UInt absPartIdx, Bool bRD)
{
    if (bRD)
    {
        absPartIdx = 0;
    }

    m_pcEntropyCoderIf->codeIntraDirChroma(cu, absPartIdx);
}

Void TEncEntropy::encodePredInfo(TComDataCU* cu, UInt absPartIdx, Bool bRD)
{
    if (bRD)
    {
        absPartIdx = 0;
    }
    if (cu->isIntra(absPartIdx))                                  // If it is Intra mode, encode intra prediction mode.
    {
        encodeIntraDirModeLuma(cu, absPartIdx, true);
        encodeIntraDirModeChroma(cu, absPartIdx, bRD);
    }
    else                                                              // if it is Inter mode, encode motion vector and reference index
    {
        encodePUWise(cu, absPartIdx, bRD);
    }
}

/** encode motion information for every PU block
 * \param cu
 * \param absPartIdx
 * \param bRD
 * \returns Void
 */
Void TEncEntropy::encodePUWise(TComDataCU* cu, UInt absPartIdx, Bool bRD)
{
    if (bRD)
    {
        absPartIdx = 0;
    }

    PartSize partSize = cu->getPartitionSize(absPartIdx);
    UInt uiNumPU = (partSize == SIZE_2Nx2N ? 1 : (partSize == SIZE_NxN ? 4 : 2));
    UInt depth = cu->getDepth(absPartIdx);
    UInt uiPUOffset = (g_auiPUOffset[UInt(partSize)] << ((cu->getSlice()->getSPS()->getMaxCUDepth() - depth) << 1)) >> 4;

    for (UInt partIdx = 0, uiSubPartIdx = absPartIdx; partIdx < uiNumPU; partIdx++, uiSubPartIdx += uiPUOffset)
    {
        encodeMergeFlag(cu, uiSubPartIdx);
        if (cu->getMergeFlag(uiSubPartIdx))
        {
            encodeMergeIndex(cu, uiSubPartIdx);
        }
        else
        {
            encodeInterDirPU(cu, uiSubPartIdx);
            for (UInt uiRefListIdx = 0; uiRefListIdx < 2; uiRefListIdx++)
            {
                if (cu->getSlice()->getNumRefIdx(RefPicList(uiRefListIdx)) > 0)
                {
                    encodeRefFrmIdxPU(cu, uiSubPartIdx, RefPicList(uiRefListIdx));
                    encodeMvdPU(cu, uiSubPartIdx, RefPicList(uiRefListIdx));
                    encodeMVPIdxPU(cu, uiSubPartIdx, RefPicList(uiRefListIdx));
                }
            }
        }
    }
}

Void TEncEntropy::encodeInterDirPU(TComDataCU* cu, UInt absPartIdx)
{
    if (!cu->getSlice()->isInterB())
    {
        return;
    }

    m_pcEntropyCoderIf->codeInterDir(cu, absPartIdx);
}

/** encode reference frame index for a PU block
 * \param cu
 * \param absPartIdx
 * \param eRefList
 * \returns Void
 */
Void TEncEntropy::encodeRefFrmIdxPU(TComDataCU* cu, UInt absPartIdx, RefPicList eRefList)
{
    assert(!cu->isIntra(absPartIdx));
    {
        if ((cu->getSlice()->getNumRefIdx(eRefList) == 1))
        {
            return;
        }

        if (cu->getInterDir(absPartIdx) & (1 << eRefList))
        {
            m_pcEntropyCoderIf->codeRefFrmIdx(cu, absPartIdx, eRefList);
        }
    }
}

/** encode motion vector difference for a PU block
 * \param cu
 * \param absPartIdx
 * \param eRefList
 * \returns Void
 */
Void TEncEntropy::encodeMvdPU(TComDataCU* cu, UInt absPartIdx, RefPicList eRefList)
{
    assert(!cu->isIntra(absPartIdx));

    if (cu->getInterDir(absPartIdx) & (1 << eRefList))
    {
        m_pcEntropyCoderIf->codeMvd(cu, absPartIdx, eRefList);
    }
}

Void TEncEntropy::encodeMVPIdxPU(TComDataCU* cu, UInt absPartIdx, RefPicList eRefList)
{
    if ((cu->getInterDir(absPartIdx) & (1 << eRefList)))
    {
        m_pcEntropyCoderIf->codeMVPIdx(cu, absPartIdx, eRefList);
    }
}

Void TEncEntropy::encodeQtCbf(TComDataCU* cu, UInt absPartIdx, TextType ttype, UInt trDepth)
{
    m_pcEntropyCoderIf->codeQtCbf(cu, absPartIdx, ttype, trDepth);
}

Void TEncEntropy::encodeTransformSubdivFlag(UInt uiSymbol, UInt uiCtx)
{
    m_pcEntropyCoderIf->codeTransformSubdivFlag(uiSymbol, uiCtx);
}

Void TEncEntropy::encodeQtRootCbf(TComDataCU* cu, UInt absPartIdx)
{
    m_pcEntropyCoderIf->codeQtRootCbf(cu, absPartIdx);
}

Void TEncEntropy::encodeQtCbfZero(TComDataCU* cu, TextType ttype, UInt trDepth)
{
    m_pcEntropyCoderIf->codeQtCbfZero(cu, ttype, trDepth);
}

Void TEncEntropy::encodeQtRootCbfZero(TComDataCU* cu)
{
    m_pcEntropyCoderIf->codeQtRootCbfZero(cu);
}

// dQP
Void TEncEntropy::encodeQP(TComDataCU* cu, UInt absPartIdx, Bool bRD)
{
    if (bRD)
    {
        absPartIdx = 0;
    }

    if (cu->getSlice()->getPPS()->getUseDQP())
    {
        m_pcEntropyCoderIf->codeDeltaQP(cu, absPartIdx);
    }
}

// texture

/** encode coefficients
 * \param cu
 * \param absPartIdx
 * \param depth
 * \param width
 * \param height
 */
Void TEncEntropy::encodeCoeff(TComDataCU* cu, UInt absPartIdx, UInt depth, UInt width, UInt height, Bool& bCodeDQP)
{
    UInt uiMinCoeffSize = cu->getPic()->getMinCUWidth() * cu->getPic()->getMinCUHeight();
    UInt uiLumaOffset   = uiMinCoeffSize * absPartIdx;
    UInt uiChromaOffset = uiLumaOffset >> 2;

    if (cu->isIntra(absPartIdx))
    {
        DTRACE_CABAC_VL(g_nSymbolCounter++)
        DTRACE_CABAC_T("\tdecodeTransformIdx()\tCUDepth=")
        DTRACE_CABAC_V(depth)
        DTRACE_CABAC_T("\n")
    }
    else
    {
        if (!(cu->getMergeFlag(absPartIdx) && cu->getPartitionSize(absPartIdx) == SIZE_2Nx2N))
        {
            m_pcEntropyCoderIf->codeQtRootCbf(cu, absPartIdx);
        }
        if (!cu->getQtRootCbf(absPartIdx))
        {
            return;
        }
    }

    xEncodeTransform(cu, uiLumaOffset, uiChromaOffset, absPartIdx, depth, width, height, 0, bCodeDQP);
}

Void TEncEntropy::encodeCoeffNxN(TComDataCU* cu, TCoeff* pcCoeff, UInt absPartIdx, UInt uiTrWidth, UInt uiTrHeight, UInt depth, TextType ttype)
{
    // This is for Transform unit processing. This may be used at mode selection stage for Inter.
    m_pcEntropyCoderIf->codeCoeffNxN(cu, pcCoeff, absPartIdx, uiTrWidth, uiTrHeight, depth, ttype);
}

Void TEncEntropy::estimateBit(estBitsSbacStruct* pcEstBitsSbac, Int width, Int height, TextType eTType)
{
    eTType = eTType == TEXT_LUMA ? TEXT_LUMA : TEXT_CHROMA;

    m_pcEntropyCoderIf->estBit(pcEstBitsSbac, width, height, eTType);
}

/** Encode SAO Offset
 * \param  saoLcuParam SAO LCU paramters
 */
Void TEncEntropy::encodeSaoOffset(SaoLcuParam* saoLcuParam, UInt compIdx)
{
    UInt uiSymbol;
    Int i;

    uiSymbol = saoLcuParam->typeIdx + 1;
    if (compIdx != 2)
    {
        m_pcEntropyCoderIf->codeSaoTypeIdx(uiSymbol);
    }
    if (uiSymbol)
    {
        if (saoLcuParam->typeIdx < 4 && compIdx != 2)
        {
            saoLcuParam->subTypeIdx = saoLcuParam->typeIdx;
        }
        Int bitDepth = compIdx ? g_bitDepthC : g_bitDepthY;
        Int offsetTh = 1 << min(bitDepth - 5, 5);
        if (saoLcuParam->typeIdx == SAO_BO)
        {
            for (i = 0; i < saoLcuParam->length; i++)
            {
                UInt absOffset = ((saoLcuParam->offset[i] < 0) ? -saoLcuParam->offset[i] : saoLcuParam->offset[i]);
                m_pcEntropyCoderIf->codeSaoMaxUvlc(absOffset, offsetTh - 1);
            }

            for (i = 0; i < saoLcuParam->length; i++)
            {
                if (saoLcuParam->offset[i] != 0)
                {
                    UInt sign = (saoLcuParam->offset[i] < 0) ? 1 : 0;
                    m_pcEntropyCoderIf->codeSAOSign(sign);
                }
            }

            uiSymbol = (UInt)(saoLcuParam->subTypeIdx);
            m_pcEntropyCoderIf->codeSaoUflc(5, uiSymbol);
        }
        else if (saoLcuParam->typeIdx < 4)
        {
            m_pcEntropyCoderIf->codeSaoMaxUvlc(saoLcuParam->offset[0], offsetTh - 1);
            m_pcEntropyCoderIf->codeSaoMaxUvlc(saoLcuParam->offset[1], offsetTh - 1);
            m_pcEntropyCoderIf->codeSaoMaxUvlc(-saoLcuParam->offset[2], offsetTh - 1);
            m_pcEntropyCoderIf->codeSaoMaxUvlc(-saoLcuParam->offset[3], offsetTh - 1);
            if (compIdx != 2)
            {
                uiSymbol = (UInt)(saoLcuParam->subTypeIdx);
                m_pcEntropyCoderIf->codeSaoUflc(2, uiSymbol);
            }
        }
    }
}

/** Encode SAO unit interleaving
* \param  rx
* \param  ry
* \param  pSaoParam
* \param  cu
* \param  iCUAddrInSlice
* \param  iCUAddrUpInSlice
* \param  bLFCrossSliceBoundaryFlag
 */
Void TEncEntropy::encodeSaoUnitInterleaving(Int compIdx, Bool saoFlag, Int rx, Int ry, SaoLcuParam* saoLcuParam, Int cuAddrInSlice, Int cuAddrUpInSlice, Int allowMergeLeft, Int allowMergeUp)
{
    if (saoFlag)
    {
        if (rx > 0 && cuAddrInSlice != 0 && allowMergeLeft)
        {
            m_pcEntropyCoderIf->codeSaoMerge(saoLcuParam->mergeLeftFlag);
        }
        else
        {
            saoLcuParam->mergeLeftFlag = 0;
        }
        if (saoLcuParam->mergeLeftFlag == 0)
        {
            if ((ry > 0) && (cuAddrUpInSlice >= 0) && allowMergeUp)
            {
                m_pcEntropyCoderIf->codeSaoMerge(saoLcuParam->mergeUpFlag);
            }
            else
            {
                saoLcuParam->mergeUpFlag = 0;
            }
            if (!saoLcuParam->mergeUpFlag)
            {
                encodeSaoOffset(saoLcuParam, compIdx);
            }
        }
    }
}

Int TEncEntropy::countNonZeroCoeffs(TCoeff* pcCoef, UInt uiSize)
{
    Int count = 0;

    for (Int i = 0; i < uiSize; i++)
    {
        count += pcCoef[i] != 0;
    }

    return count;
}

/** encode quantization matrix
 * \param scalingList quantization matrix information
 */
Void TEncEntropy::encodeScalingList(TComScalingList* scalingList)
{
    m_pcEntropyCoderIf->codeScalingList(scalingList);
}

//! \}
