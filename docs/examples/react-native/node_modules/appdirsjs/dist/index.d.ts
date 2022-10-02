declare type Options = {
    appName: string;
    legacyPath?: string;
};
declare type Directories = {
    cache: string;
    config: string;
    data: string;
    runtime?: string;
};
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
export default function appDirs(options: Options): Directories;
export {};
