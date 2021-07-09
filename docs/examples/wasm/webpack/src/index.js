import themis from 'wasm-themis'
import { setupSecureCell } from './secure-cell.js'

window.onload = function() {
    themis.initialize().then(function() {
        const loaded = document.getElementById('wasm-loaded')
        loaded.textContent = 'WasmThemis loaded!'
        loaded.classList.add('dimmed')
        setupSecureCell()
    })
}
