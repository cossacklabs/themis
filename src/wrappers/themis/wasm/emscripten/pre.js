// Manually export Emscripten runtime functions for weird environments that
// look like Node.js, but aren't Node.js (e.g., Electron apps or output of
// web bundlers like webpack or Browserify)
module.exports = Module;
