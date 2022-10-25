
import { StyleSheet } from "react-native";
import { isDarkMode } from "./utils";

export const styles = StyleSheet.create({
    title: {
        fontSize: 20,
        fontWeight: '600',
        color: isDarkMode() ? 'white' : 'black',
    },
    resolved: {
        color: 'green',
        fontFamily: Platform.OS === 'ios' ? 'Courier' : 'monospace',
    },
    rejected: {
        color: 'red',
        fontFamily: Platform.OS === 'ios' ? 'Courier' : 'monospace',
    },
    text: {
        color: isDarkMode() ? 'white' : 'black',
    },
    warning: {
        color: 'orange',
    },
    sectionContainer: {
        marginTop: 32,
        paddingHorizontal: 24,
    },
    sectionTitle: {
        fontSize: 24,
        fontWeight: '600',
    },
    sectionDescription: {
        marginTop: 8,
        fontSize: 18,
        fontWeight: '400',
    },
    highlight: {
        fontWeight: '700',
    },
    blob: {
        fontSize: 14,
        fontFamily: Platform.OS === 'ios' ? 'Courier' : 'monospace',
        flexWrap: 'nowrap',
        flexShrink: 1,
    },
    resolved: {
        color: 'green',
        fontSize: 18,
    },
    rejected: {
        color: 'red',
        fontSize: 18,
    },
    row: {
        flex: 1,
        flexDirection: 'row',
        flexWrap: 'wrap',
        alignItems: 'flex-start',
    },
    col50: {
        width: '50%',
    }
});
