"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const fs = require("fs");
const os = require("os");
const path = require("path");
/**
 * Returns application-specific paths for directories.
 *
 * For Linux, it returns paths according to
 * [XDG Base Directory Specification](https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html)
 *
 * For MacOS, it returns paths according to
 * [Apple Technical Q&A](https://developer.apple.com/library/archive/qa/qa1170/_index.html#//apple_ref/doc/uid/DTS10001702)
 *
 * For Windows, it returns paths according to
 * [Stackoverflow's answer](https://stackoverflow.com/questions/43853548/xdg-basedir-directories-for-windows)
 *
 * Specific information about each OS can be found
 * in corresponding functions.
 *
 * @param appName  Application name
 * @param legacyPath  Path to provide backward compability
 *                    and not to force users to move files.
 *                    Will be used if exists if filesystem.
 */
function appDirs(options) {
    if (process.platform === "linux") {
        return linux(options);
    }
    else if (process.platform === "win32") {
        return windows(options);
    }
    else if (process.platform === "darwin") {
        return macos(options);
    }
    return fallback(options);
}
exports.default = appDirs;
function fallback({ appName, legacyPath }) {
    console.warn(`[appdirsjs]: can't get directories for "${process.platform}" platform, using fallback values`);
    function fallbackPath() {
        if (legacyPath) {
            return legacyPath;
        }
        // Sane default for Unix-like systems
        return path.join(os.homedir(), "." + appName);
    }
    return Object.freeze({
        cache: fallbackPath(),
        config: fallbackPath(),
        data: fallbackPath(),
    });
}
function linux({ appName, legacyPath }) {
    const home = os.homedir();
    const uid = process.getuid();
    const env = process.env;
    function xdgPath(allowLegacy, env, defaultRoot) {
        if (allowLegacy && legacyPath && fs.existsSync(legacyPath)) {
            return legacyPath;
        }
        const root = env || defaultRoot;
        return path.join(root, appName);
    }
    return Object.freeze({
        cache: xdgPath(true, env.XDG_CACHE_HOME, path.join(home, ".cache")),
        config: xdgPath(true, env.XDG_CONFIG_HOME, path.join(home, ".config")),
        data: xdgPath(true, env.XDG_DATA_HOME, path.join(home, ".local", "share")),
        runtime: xdgPath(false, env.XDG_RUNTIME_DIR, path.join("/run", "user", uid.toString())),
    });
}
function windows({ appName, legacyPath }) {
    if (legacyPath && fs.existsSync(legacyPath)) {
        return Object.freeze({
            cache: legacyPath,
            config: legacyPath,
            data: legacyPath,
        });
    }
    const home = os.homedir();
    const roamingAppData = process.env.APPDATA || path.join(home, "AppData", "Roaming");
    const localAppData = process.env.LOCALAPPDATA || path.join(home, "AppData", "Local");
    return Object.freeze({
        cache: path.join(localAppData, "Temp", appName),
        config: path.join(roamingAppData, appName),
        data: path.join(localAppData, appName),
    });
}
function macos({ appName, legacyPath }) {
    if (legacyPath && fs.existsSync(legacyPath)) {
        return Object.freeze({
            cache: legacyPath,
            config: legacyPath,
            data: legacyPath,
        });
    }
    const home = os.homedir();
    return Object.freeze({
        cache: path.join(home, "Library", "Caches", appName),
        config: path.join(home, "Library", "Preferences", appName),
        data: path.join(home, "Library", "Application Support", appName),
    });
}
