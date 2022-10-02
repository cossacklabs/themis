# appdirsjs

[![GitHub Workflow Status](https://img.shields.io/github/workflow/status/codingjerk/appdirsjs/ci)](https://github.com/codingjerk/appdirsjs/actions)
[![Codecov](https://img.shields.io/codecov/c/gh/codingjerk/appdirsjs)](https://codecov.io/gh/codingjerk/appdirsjs)
[![npm](https://img.shields.io/npm/v/appdirsjs)](https://www.npmjs.com/package/appdirsjs)
[![npm bundle size](https://img.shields.io/bundlephobia/min/appdirsjs)](https://www.npmjs.com/package/appdirsjs)
[![GitHub](https://img.shields.io/github/license/codingjerk/appdirsjs)](https://github.com/codingjerk/appdirsjs/blob/master/LICENSE.md)

A node.js library to get paths to directories to store configs, caches and data according to OS standarts.

## Installation

```sh
npm install appdirsjs
```

or

```sh
yarn install appdirsjs
```

if you're using yarn.

## Usage

```javascript
import appDirs from "appdirsjs";

const dirs = appDirs({ appName: "expo" });

console.log(dirs.cache);
// /home/user/.cache/expo on Linux
// /Users/User/Library/Caches/expo on MacOS
// C:\Users\User\AppData\Local\Temp\expo on Windows

console.log(dirs.config);
// /home/user/.config/expo on Linux
// /Users/User/Library/Preferences/expo on MacOS
// C:\Users\User\AppData\Roaming\expo

console.log(dirs.data);
// /home/user/.local/share/expo on Linux
// /Users/User/Library/Application Support/expo on MacOS
// C:\Users\User\AppData\Local\expo
```

### Keep backward compability

Then switching from old-style dotfile directory,
such as `~/.myapp` to new, like `~/.config/myapp`,
you can pass `legacyPath` parameter
to keep using old directory if it exists:

```javascript
import * as path from "path";
import appDirs from "appdirsjs";

const dirs = appDirs({
  appName: "expo",
  // Notice usage of full path
  legacyPath: path.join(os.homedir(), ".expo"),
});

console.log(dirs.config);
// /home/user/.expo
```

## TODO

- [ ] Android support
- [ ] XDG on BSD support
