// words.h [INTERFACE]
// A module that provides max_words secret words
//   numbered 1...max_words

extern const int max_words;

// get_word(n) returns word #n
// requires: 1 <= n <= max_words
// time: O(1)
const char *get_word(int n);
