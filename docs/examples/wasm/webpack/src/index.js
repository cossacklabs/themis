// FIXME: use 'wasm-themis' instead of '@ilammy/wasm-themis-preview'
import themis from '@ilammy/wasm-themis-preview'
import { setupSecureCell } from './secure-cell.js'

window.onload = function() {
    themis.initialized.then(function() {
        const loaded = document.getElementById('wasm-loaded')
        loaded.textContent = 'WasmThemis loaded!'
        loaded.classList.add('dimmed')
        setupSecureCell()
    })
}
