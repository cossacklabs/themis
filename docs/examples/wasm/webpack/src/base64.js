/*
 * https://developer.mozilla.org/en-US/docs/Web/JavaScript/Base64_encoding_and_decoding
 */

function b64ToUint6(n) {
    return n > 64 && n < 91  ? n - 65
         : n > 96 && n < 123 ? n - 71
         : n > 47 && n < 58  ? n + 4
         : n === 43 ? 62
         : n === 47 ? 63
         : 0
}

function uint6ToB64(c) {
    return c < 26 ? c + 65
         : c < 52 ? c + 71
         : c < 62 ? c - 4
         : c === 62 ? 43
         : c === 63 ? 47
         : 65
}

export function Base64ToBytes(sBase64, nBlocksSize) {
    const
        sB64Enc = sBase64.replace(/[^A-Za-z0-9\+\/]/g, ""),
        nInLen  = sB64Enc.length,
        nOutLen = nBlocksSize ? Math.ceil((nInLen * 3 + 1 >> 2) / nBlocksSize) * nBlocksSize
                              : nInLen * 3 + 1 >> 2,
        taBytes = new Uint8Array(nOutLen);

    for (let nMod3, nMod4, nUint24 = 0, nOutIdx = 0, nInIdx = 0; nInIdx < nInLen; nInIdx++) {
        nMod4 = nInIdx & 3
        nUint24 |= b64ToUint6(sB64Enc.charCodeAt(nInIdx)) << 6 * (3 - nMod4)
        if (nMod4 === 3 || nInLen - nInIdx === 1) {
            for (nMod3 = 0; nMod3 < 3 && nOutIdx < nOutLen; nMod3++, nOutIdx++) {
                taBytes[nOutIdx] = nUint24 >>> (16 >>> nMod3 & 24) & 255
            }
            nUint24 = 0
        }
    }
    return taBytes
}

export function BytesToBase64(aBytes, maxWidth) {
    let nMod3 = 2, sB64Enc = "";
    if (!maxWidth) { maxWidth = 44 }
    for (var nLen = aBytes.length, nUint24 = 0, nIdx = 0; nIdx < nLen; nIdx++) {
        nMod3 = nIdx % 3
        if (nIdx > 0 && (nIdx * 4 / 3) % maxWidth === 0) { sB64Enc += "\r\n" }
        nUint24 |= aBytes[nIdx] << (16 >>> nMod3 & 24)
        if (nMod3 === 2 || aBytes.length - nIdx === 1) {
            sB64Enc += String.fromCharCode(uint6ToB64(nUint24 >>> 18 & 63),
                                           uint6ToB64(nUint24 >>> 12 & 63),
                                           uint6ToB64(nUint24 >>> 6 & 63),
                                           uint6ToB64(nUint24 & 63))
            nUint24 = 0
        }
    }
    return sB64Enc.substr(0, sB64Enc.length - 2 + nMod3) +
           (nMod3 === 2 ? '' : nMod3 === 1 ? '=' : '==')
}
