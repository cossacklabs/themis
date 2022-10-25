import {
    Appearance
} from 'react-native';

export function isDarkMode() {
    const colorScheme = Appearance.getColorScheme();
    return colorScheme === 'dark';
}
