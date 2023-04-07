#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "hangman-io.h"
#include "words.h"

//const int LOWERCASE = 32;
const int MAX_STRIKES = 6;

// check_word(secret_word, asterisk_word, input_char) checks if input is within word
// requires: secret_word & asterisk_word are valid
// effects: may modify asterist_word
// time: O(n)
bool check_word(const char secret_word[], char asterisk_word[], char input_char) {
  assert(secret_word);
  assert(asterisk_word);
  bool correct_guess = false;
  for (int i = 0; secret_word[i] != '\0'; ++i) {
    if (input_char == secret_word[i] || 
          input_char == secret_word[i] - 'A' + 'a') {
      asterisk_word[i] = secret_word[i];
      correct_guess = true;
    }
  }
  return correct_guess;
}

// retry() checks whether the player wants to retry the game
// effects: produces output
bool retry(void) {
  char input = '\0';
  while (true) {
      printf("%s", play_again);
      scanf(" %c", &input);
    if (input == 'Y' || input == 'y') {
      return true;
    } else if (input == 'N' || input == 'n') {
      return false;
    } else {
      printf("%s", invalid_play_again_response);
    }
  }
}

// is_uppercase(c) checks whether the given character is upper case
bool is_uppercase(char c) {
  if ((c >= 'A') && (c <= 'Z')) {
    return true;
  } else {
    return false;
  }
}

int main(void) {
  while (true) {
    bool ongoing_game = false;
    bool loss = false;
    bool win = false;
    int game_num = 0;
    int strike_num = 0;
    // Checking whether it is a new game
    if (!ongoing_game) {
        game_num = 0;
        strike_num = 0;
        printf(enter_game_number, max_words);
        scanf("%d", &game_num);
        if (0 < game_num && game_num <= max_words) {
          ongoing_game = true;
        } else {
          printf("%s", invalid_game_number);
          continue;
        }
    }
    // Starting new game
    if (ongoing_game) {
      // Gets the word from get_word
      const char *secret_word = get_word(game_num);
      char asterisk_word[51] = {'\0'};
      strcpy(asterisk_word, secret_word);

      // Replaces letters with asterisks
      for (int i = 0; secret_word[i] != '\0'; ++i) {
        if (asterisk_word[i] != ' ') {
          asterisk_word[i] = '*';
        }
      }

      // Keeps the history of attempts
      char attempts[51] = {'\0'};
      int attempt_num = 0;
      bool input_used = false;
      
      // Main game loop
      while (!loss || !win) {
        char input_char = '\0';
        print_hangman(strike_num);
        printf("%s", letter_prompt);
        printf("%s\n", asterisk_word);
        scanf(" %c", &input_char);

        // Check if the input has been used before
        for (int i = 0; i < 51; ++i) {
          if (is_uppercase(input_char) && 
              (attempts[i] - ('a' - 'A') == input_char || attempts[i] == input_char)) {
            printf(already_guessed_letter, input_char);
            input_used = true;
            break;
          } else if (!is_uppercase(input_char) && 
              (attempts[i] - ('a' - 'A') == input_char || attempts[i] == input_char)) {
            printf(already_guessed_letter, input_char - ('a' - 'A'));
            input_used = true;
            break;
          } else {
            input_used = false;
          }
        }

        // Check if input is valid
        //printf("This is the input char: %c\n", input_char);
        if (input_used) {
          continue;
        } else if (!(('A' <= input_char && input_char <= 'Z') ||
              ('a' <= input_char && input_char <= 'z'))) {
          printf(not_a_letter, input_char);
          continue;
        }

        // Checks if char is in word
        if (check_word(secret_word, asterisk_word, input_char) &&
              !(strcmp(asterisk_word, secret_word) == 0)) {
          // Adds attempt to the history
          attempts[attempt_num] = input_char;
          ++attempt_num;
          continue;
        } else if (strcmp(asterisk_word, secret_word) == 0) { // Check winstate
          printf(congratulations, secret_word);
          win = true;
          break;
        } else if (is_uppercase(input_char) && !input_used) {
          printf(not_in_word, input_char);
          ++strike_num;
          // Adds attempt to the history
          attempts[attempt_num] = input_char;
          ++attempt_num;
        } else if (!is_uppercase(input_char) && !input_used){
          // changes to upper case
          //printf("The upper case letter is %c\n", input_char - 'a' + 'A');
          printf(not_in_word, input_char - 'a' + 'A');
          ++strike_num;
          // Adds attempt to the history
          attempts[attempt_num] = input_char;
          ++attempt_num;
        } else {
          continue;
        }
      

        // Checking failstate
        if (strike_num < 6) {
          printf(not_last_guess, MAX_STRIKES - strike_num);
          continue;
        } else {
          print_hangman(6);
          printf(game_over, secret_word);
          loss = true;
          break;
        }
      }
    }
    
    // Checking retry?
    if ((loss || win) && retry()) {
      ongoing_game = false;
    } else {
      break;
    }
  }
}
