#ifndef STRINGUTILITY_HPP
#define STRINGUTILITY_HPP
#include <iostream>
inline int getLength(const char* str) {
    int len = 0;
    for (int i = 0; *(str + i) != '\0'; i++) {
        len++;
    }
    return len;
}
inline void copyStr(char* newStr, const char* oldStr) {
    int i = 0;
    for (; *(oldStr + i) != '\0'; i++) {
        *(newStr + i) = *(oldStr + i);
    }
    *(newStr + i) = '\0';
}
inline bool isEqual(const char* str1, const char* str2) {
	bool equal = true;
	int len1 = getLength(str1);
	int len2 = getLength(str2);
	if (len1 != len2) {
		equal = false;
	}
	else {
		for (int i = 0; str1[i] != '\0'; i++) {
			if (*(str1 + i) != *(str2 + i)) {
				equal = false;
				break;
			}
		}
	}
	return equal;
}
inline void convertToLower(char* str) {
	for (int i = 0; str[i] != '\0'; i++) {
		if (*(str + i) >= 65 && *(str + i) <= 90) {
			*(str + i) = *(str + i) + 32;
		}
	}
}
inline void readFile(const char* l, int& index, char* str) {
    int i = 0;
    while (*(l+index) != ',' && *(l + index) != '\0' && *(l + index) != '\n' 
		&& *(l + index) != '\r') {
		*(str +i) = *(l + index);
        i++;
        index++;
    }
	*(str + i) = '\0';
    if (*(l + index) == ',') {
        index++;
    }
}
inline float stringToFloat(const char* str) {
    float wholePart = 0.0f;
    float fractionPart = 0.0f;
    float multiplier = 0.1f;
    int i = 0;
    while (str[i] != '.' && str[i] != '\0') {
        wholePart = (wholePart * 10.0f) + (str[i] - '0');
        i++;
    }
    if (str[i] == '.') {
        i++; 
        while (str[i] != '\0') {
            fractionPart = fractionPart + (str[i] - '0') * multiplier;
            multiplier = multiplier * 0.1f;
            i++;
        }
    }
    return wholePart + fractionPart;
}

#endif
