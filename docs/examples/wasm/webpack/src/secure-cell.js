import themis from 'wasm-themis'
import { Base64ToBytes, BytesToBase64 } from './base64.js'
import { StringToUTF8Array, UTF8ArrayToString } from './utf8.js'

export function setupSecureCell() {
    const secureCellSection = document.getElementById('secure-cell-section')

    const symmetricKey = document.getElementById('secure-cell-symmetric-key')
    const symmetricKeyRadio = symmetricKey.querySelector('input[type=radio]')
    const symmetricKeyInput = symmetricKey.querySelector('input[type=text]')
    const symmetricKeyLength = symmetricKey.querySelector('span.size-hint')
    const generateButton = symmetricKey.querySelector('input[type=button]')

    const passphrase = document.getElementById('secure-cell-passphrase')
    const passphraseRadio = passphrase.querySelector('input[type=radio]')
    const passphraseInput = passphrase.querySelector('input[type=text]')
    const passphraseLength = passphrase.querySelector('span.size-hint')

    const plaintext = document.getElementById('secure-cell-plaintext')
    const plaintextArea = plaintext.querySelector('textarea')
    const plaintextLength = plaintext.querySelector('span.size-hint')

    const context = document.getElementById('secure-cell-context')
    const contextArea = context.querySelector('textarea')
    const contextLength = context.querySelector('span.size-hint')

    const ciphertextSeal = document.getElementById('secure-cell-seal-ciphertext')
    const ciphertextSealArea = ciphertextSeal.querySelector('textarea')
    const ciphertextSealLength = ciphertextSeal.querySelector('span.size-hint')

    const outputBoxSeal = document.getElementById('secure-cell-seal-output')
    const errorBoxSeal = outputBoxSeal.querySelector('div.error-box')

    const ciphertextTokenProtect = document.getElementById('secure-cell-token-protect-ciphertext')
    const ciphertextTokenProtectArea = ciphertextTokenProtect.querySelector('textarea')
    const ciphertextTokenProtectLength = ciphertextTokenProtect.querySelector('span.size-hint')

    const authTokenTokenProtect = document.getElementById('secure-cell-token-protect-auth-token')
    const authTokenTokenProtectArea = authTokenTokenProtect.querySelector('textarea')
    const authTokenTokenProtectLength = authTokenTokenProtect.querySelector('span.size-hint')

    const outputBoxTokenProtect = document.getElementById('secure-cell-token-protect-output')
    const errorBoxTokenProtect = outputBoxTokenProtect.querySelector('div.error-box')

    const ciphertextContextImprint = document.getElementById('secure-cell-context-imprint-ciphertext')
    const ciphertextContextImprintArea = ciphertextContextImprint.querySelector('textarea')
    const ciphertextContextImprintLength = ciphertextContextImprint.querySelector('span.size-hint')

    const outputBoxContextImprint = document.getElementById('secure-cell-context-imprint-output')
    const errorBoxContextImprint = outputBoxContextImprint.querySelector('div.error-box')

    function adjustTextAreaRows(area) {
        // Non-BMP characters? Grapheme clusters? Emoji? Who uses *that*?
        const rows = area.value.split('\n').reduce(((rows, row) => {
            return rows + Math.floor((row.length + area.cols - 1) / area.cols)
        }), 0)
        if (rows <= 5 && area.rows < rows) {
            area.rows = rows
        }
    }

    function massageErrorMessage(error) {
        let message = error.toString()
        const colonIndex = message.lastIndexOf(': ')
        if (colonIndex != -1) {
            message = message.slice(0, colonIndex) + ':<br>' + message.slice(colonIndex + 2)
        }
        return message
    }

    function encryptSealWithKey() {
        try {
            const decodedKey = Base64ToBytes(symmetricKeyInput.value)
            const encodedContext = StringToUTF8Array(contextArea.value)
            const encodedPlaintext = StringToUTF8Array(plaintextArea.value)

            const cell = themis.SecureCellSeal.withKey(decodedKey)
            const ciphertext = cell.encrypt(encodedPlaintext, encodedContext)

            ciphertextSealArea.value = BytesToBase64(ciphertext)
            ciphertextSealLength.textContent = `${ciphertext.length} bytes`
            adjustTextAreaRows(ciphertextSealArea)

            errorBoxSeal.textContent = ''
            errorBoxSeal.classList.add('hidden')
        }
        catch (error) {
            ciphertextSealArea.value = ''
            ciphertextSealLength.textContent = '0 bytes'

            errorBoxSeal.innerHTML = massageErrorMessage(error)
            errorBoxSeal.classList.remove('hidden')
        }
    }

    function decryptSealWithKey() {
        try {
            const decodedKey = Base64ToBytes(symmetricKeyInput.value)
            const encodedContext = StringToUTF8Array(contextArea.value)
            const decodedCiphertext = Base64ToBytes(ciphertextSealArea.value)

            const cell = themis.SecureCellSeal.withKey(decodedKey)
            const plaintext = cell.decrypt(decodedCiphertext, encodedContext)

            plaintextArea.value = UTF8ArrayToString(plaintext)
            plaintextLength.textContent = `${plaintext.length} bytes`
            adjustTextAreaRows(plaintextArea)

            errorBoxSeal.textContent = ''
            errorBoxSeal.classList.add('hidden')
        }
        catch (error) {
            plaintextArea.value = ''
            plaintextLength.textContent = '0 bytes'

            errorBoxSeal.innerHTML = massageErrorMessage(error)
            errorBoxSeal.classList.remove('hidden')
        }
    }

    function encryptSealPassphrase() {
        try {
            const encodedPassphrase = StringToUTF8Array(passphraseInput.value)
            const encodedContext = StringToUTF8Array(contextArea.value)
            const encodedPlaintext = StringToUTF8Array(plaintextArea.value)

            const cell = themis.SecureCellSeal.withPassphrase(encodedPassphrase)
            const ciphertext = cell.encrypt(encodedPlaintext, encodedContext)

            ciphertextSealArea.value = BytesToBase64(ciphertext)
            ciphertextSealLength.textContent = `${ciphertext.length} bytes`
            adjustTextAreaRows(ciphertextSealArea)

            errorBoxSeal.textContent = ''
            errorBoxSeal.classList.add('hidden')
        }
        catch (error) {
            ciphertextSealArea.value = ''
            ciphertextSealLength.textContent = '0 bytes'

            errorBoxSeal.innerHTML = massageErrorMessage(error)
            errorBoxSeal.classList.remove('hidden')
        }
    }

    function decryptSealPassphrase() {
        try {
            const encodedPassphrase = StringToUTF8Array(passphraseInput.value)
            const encodedContext = StringToUTF8Array(contextArea.value)
            const decodedCiphertext = Base64ToBytes(ciphertextSealArea.value)

            const cell = themis.SecureCellSeal.withPassphrase(encodedPassphrase)
            const plaintext = cell.decrypt(decodedCiphertext, encodedContext)

            plaintextArea.value = UTF8ArrayToString(plaintext)
            plaintextLength.textContent = `${plaintext.length} bytes`
            adjustTextAreaRows(plaintextArea)

            errorBoxSeal.textContent = ''
            errorBoxSeal.classList.add('hidden')
        }
        catch (error) {
            plaintextArea.value = ''
            plaintextLength.textContent = '0 bytes'

            errorBoxSeal.innerHTML = massageErrorMessage(error)
            errorBoxSeal.classList.remove('hidden')
        }
    }

    function encryptTokenProtect() {
        try {
            const decodedKey = Base64ToBytes(symmetricKeyInput.value)
            const encodedContext = StringToUTF8Array(contextArea.value)
            const encodedPlaintext = StringToUTF8Array(plaintextArea.value)

            const cell = themis.SecureCellTokenProtect.withKey(decodedKey)
            const { data, token } = cell.encrypt(encodedPlaintext, encodedContext)

            ciphertextTokenProtectArea.value = BytesToBase64(data)
            ciphertextTokenProtectLength.textContent = `${data.length} bytes`
            adjustTextAreaRows(ciphertextTokenProtectArea)

            authTokenTokenProtectArea.value = BytesToBase64(token)
            authTokenTokenProtectLength.textContent = `${token.length} bytes`
            adjustTextAreaRows(authTokenTokenProtectArea)

            errorBoxTokenProtect.textContent = ''
            errorBoxTokenProtect.classList.add('hidden')
        }
        catch (error) {
            ciphertextTokenProtectArea.value = ''
            ciphertextTokenProtectLength.textContent = '0 bytes'

            authTokenTokenProtectArea.value = ''
            authTokenTokenProtectLength.textContent = '0 bytes'

            errorBoxTokenProtect.innerHTML = massageErrorMessage(error)
            errorBoxTokenProtect.classList.remove('hidden')
        }
    }

    function decryptTokenProtect() {
        try {
            const decodedKey = Base64ToBytes(symmetricKeyInput.value)
            const encodedContext = StringToUTF8Array(contextArea.value)
            const decodedCiphertext = Base64ToBytes(ciphertextTokenProtectArea.value)
            const decodedAuthToken = Base64ToBytes(authTokenTokenProtectArea.value)

            const cell = themis.SecureCellTokenProtect.withKey(decodedKey)
            const plaintext = cell.decrypt(decodedCiphertext, decodedAuthToken, encodedContext)

            plaintextArea.value = UTF8ArrayToString(plaintext)
            plaintextLength.textContent = `${plaintext.length} bytes`
            adjustTextAreaRows(plaintextArea)

            errorBoxTokenProtect.textContent = ''
            errorBoxTokenProtect.classList.add('hidden')
        }
        catch (error) {
            plaintextArea.value = ''
            plaintextLength.textContent = '0 bytes'

            errorBoxTokenProtect.innerHTML = massageErrorMessage(error)
            errorBoxTokenProtect.classList.remove('hidden')
        }
    }

    function encryptContextImprint() {
        try {
            const decodedKey = Base64ToBytes(symmetricKeyInput.value)
            const encodedContext = StringToUTF8Array(contextArea.value)
            const encodedPlaintext = StringToUTF8Array(plaintextArea.value)

            const cell = themis.SecureCellContextImprint.withKey(decodedKey)
            const ciphertext = cell.encrypt(encodedPlaintext, encodedContext)

            ciphertextContextImprintArea.value = BytesToBase64(ciphertext)
            ciphertextContextImprintLength.textContent = `${ciphertext.length} bytes`
            adjustTextAreaRows(ciphertextContextImprintArea)

            errorBoxContextImprint.textContent = ''
            errorBoxContextImprint.classList.add('hidden')
        }
        catch (error) {
            ciphertextContextImprintArea.value = ''
            ciphertextContextImprintLength.textContent = '0 bytes'

            errorBoxContextImprint.innerHTML = massageErrorMessage(error)
            errorBoxContextImprint.classList.remove('hidden')
        }
    }

    function decryptContextImprint() {
        try {
            const decodedKey = Base64ToBytes(symmetricKeyInput.value)
            const encodedContext = StringToUTF8Array(contextArea.value)
            const decodedCiphertext = Base64ToBytes(ciphertextContextImprintArea.value)

            const cell = themis.SecureCellContextImprint.withKey(decodedKey)
            const plaintext = cell.decrypt(decodedCiphertext, encodedContext)

            plaintextArea.value = UTF8ArrayToString(plaintext)
            plaintextLength.textContent = `${plaintext.length} bytes`
            adjustTextAreaRows(plaintextArea)

            errorBoxContextImprint.textContent = ''
            errorBoxContextImprint.classList.add('hidden')
        }
        catch (error) {
            plaintextArea.value = ''
            plaintextLength.textContent = '0 bytes'

            errorBoxContextImprint.innerHTML = massageErrorMessage(error)
            errorBoxContextImprint.classList.remove('hidden')
        }
    }

    function encryptWithKey() {
        encryptSealWithKey()
        encryptTokenProtect()
        encryptContextImprint()
    }

    function encryptWithPassphrase() {
        encryptSealPassphrase()
    }

    let encrypt = encryptWithKey
    let decryptSeal = decryptSealWithKey

    let update = function(){}

    generateButton.addEventListener('click', function() {
        const key = new themis.SymmetricKey()
        symmetricKeyInput.value = BytesToBase64(key)
        symmetricKeyLength.textContent = `${key.length} bytes`
        update()
    })

    symmetricKeyInput.addEventListener('input', function() {
        const decoded = Base64ToBytes(symmetricKeyInput.value)
        symmetricKeyLength.textContent = `${decoded.length} bytes`
        update()
    })

    passphraseInput.addEventListener('input', function() {
        const decoded = Base64ToBytes(passphraseInput.value)
        passphraseLength.textContent = `${decoded.length} bytes`
        update()
    })

    let inputsEnabledForSymmetricKey = [
        symmetricKeyInput, generateButton,
        ciphertextTokenProtectArea,
        authTokenTokenProtectArea,
        ciphertextContextImprintArea,
    ]
    let inputsEnabledForPassphrase = [
        passphraseInput,
    ]
    let errorBoxes = [
        errorBoxSeal,
        errorBoxTokenProtect,
        errorBoxContextImprint,
    ]
    function secretChange() {
        if (symmetricKeyRadio.checked) {
            secureCellSection.classList.add('symmetric-key')
            secureCellSection.classList.remove('passphrase')
            inputsEnabledForSymmetricKey.forEach((input) => input.disabled = false)
            inputsEnabledForPassphrase.forEach((input) => input.disabled = true)
            encrypt = encryptWithKey
            decryptSeal = decryptSealWithKey
        }
        if (passphraseRadio.checked) {
            secureCellSection.classList.add('passphrase')
            secureCellSection.classList.remove('symmetric-key')
            inputsEnabledForSymmetricKey.forEach((input) => input.disabled = true)
            inputsEnabledForPassphrase.forEach((input) => input.disabled = false)
            encrypt = encryptWithPassphrase
            decryptSeal = decryptSealPassphrase
        }
        errorBoxes.forEach((box) => {
            box.textContent = ''
            box.classList.add('hidden')
        })
    }
    symmetricKeyRadio.addEventListener('change', secretChange)
    passphraseRadio.addEventListener('change', secretChange)
    // Some browsers persist input selection. Update "enabled/disabled" state
    // based on the actual input state right now. This does not pretty though.
    secretChange()

    plaintextArea.addEventListener('input', function() {
        const encoded = StringToUTF8Array(plaintextArea.value)
        plaintextLength.textContent = `${encoded.length} bytes`
        update = () => encrypt()
        encrypt()
    })

    contextArea.addEventListener('input', function() {
        const encoded = StringToUTF8Array(contextArea.value)
        contextLength.textContent = `${encoded.length} bytes`
        update()
    })

    ciphertextSealArea.addEventListener('input', function() {
        const encoded = StringToUTF8Array(ciphertextSealArea.value)
        ciphertextSealLength.textContent = `${encoded.length} bytes`
        update = () => decryptSeal()
        decryptSeal()
    })

    ciphertextTokenProtectArea.addEventListener('input', function() {
        const encoded = StringToUTF8Array(ciphertextTokenProtectArea.value)
        ciphertextTokenProtectLength.textContent = `${encoded.length} bytes`
        update = decryptTokenProtect
        decryptTokenProtect()
    })

    authTokenTokenProtectArea.addEventListener('input', function() {
        const encoded = StringToUTF8Array(authTokenTokenProtectArea.value)
        authTokenTokenProtectLength.textContent = `${encoded.length} bytes`
        update = decryptTokenProtect
        decryptTokenProtect()
    })

    ciphertextContextImprintArea.addEventListener('input', function() {
        const encoded = StringToUTF8Array(ciphertextContextImprintArea.value)
        ciphertextContextImprintLength.textContent = `${encoded.length} bytes`
        update = decryptContextImprint
        decryptContextImprint()
    })

    const textAreas = [
        plaintextArea,
        ciphertextSealArea,
        ciphertextTokenProtectArea,
        authTokenTokenProtectArea,
        ciphertextContextImprintArea,
    ]
    textAreas.forEach((area) => {
        area.addEventListener('input', () => adjustTextAreaRows(area))
    })
}
