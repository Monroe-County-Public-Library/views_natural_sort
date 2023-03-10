<?php

/**
 * @file
 * The Views Natural Sort module include file.
 */

/**
 * Remove all the configured words from the beginning of the string only.
 *
 * @param string $string
 *   The string we wish to transform.
 *
 * @return string
 *   The transformed string.
 */
function views_natural_sort_remove_beginning_words($string) {
  $beginning_words = config_get('views_natural_sort.settings', 'views_natural_sort_beginning_words_remove');
  if (empty($beginning_words)) {
    return $string;
  }

  array_walk($beginning_words, 'preg_quote');
  return preg_replace(
    '/^(' . implode('|', $beginning_words) . ')\s+/iu',
    '',
    $string
  );
}

/**
 * Remove all the configured words from the string.
 *
 * @param string $string
 *   The string we wish to transform.
 *
 * @return string
 *   The transformed string.
 */
function views_natural_sort_remove_words($string) {
  $words = config_get('views_natural_sort.settings', 'views_natural_sort_words_remove');
  if (empty($words)) {
    return $string;
  }

  array_walk($words, 'preg_quote');
  return preg_replace(
    array(
    '/\s(' . implode('|', $words) . ')\s+/iu',
    '/^(' . implode('|', $words) . ')\s+/iu',
  ),
    array(
    ' ',
    '',
  ),
    $string
  );
}

/**
 * Remove all the configured symbols from the string.
 *
 * @param string $string
 *   The string we wish to transform.
 *
 * @return string
 *   The transformed string.
 */
function views_natural_sort_remove_symbols($string) {
  $symbols = config_get('views_natural_sort.settings', 'views_natural_sort_symbols_remove');
  if (strlen($symbols) == 0) {
    return $string;
  }
  return preg_replace(
    '/[' . preg_quote($symbols) . ']/u',
    '',
    $string
  );
}

/**
 * Transform numbers in a string into a natural sortable string.
 *
 * Rules are as follows:
 *  - Embeded numbers will sort in numerical order. The following possibilities
 *    are supported
 *    - A leading dash indicates a negative number, unless it is preceded by a
 *      non-whitespace character, which case it is considered just a dash.
 *    - Leading zeros are properly ignored so as to not influence sort order
 *    - Decimal numbers are supported using a period as the decimal character
 *    - Thousands separates are ignored, using the comma as the thous. character
 *    - Numbers may be up to 99 digits before the decimal, up to the precision
 *      of the processor.
 *
 * @param string $string
 *   The string we wish to transform.
 */
function views_natural_sort_numbers($string) {
  // Find an optional leading dash (either preceded by whitespace or the first
  // character) followed by either:
  // - an optional series of digits (with optional embedded commas), then a
//   period, then an optional series of digits
  // - a series of digits (with optional embedded commas)
  return preg_replace_callback(
    '/(\s-|^-)?(?:(\d[\d,]*)?\.(\d+)|(\d[\d,]*))/',
    '_views_natural_sort_number_transform_match_callback',
    $string
  );
}

/**
 * Transforms a string representing numbers into a special format.
 *
 * This special format can be sorted as if it was a number but in reality is
 *   being sorted alphanumerically.
 *
 * @param array $match
 *   Array of matches passed from preg_replace_callback
 *   $match[0] is the entire matching string
 *   $match[1] if present, is the optional dash, preceded by optional whitespace
 *   $match[2] if present, is whole number portion of the decimal number
 *   $match[3] if present, is the fractional portion of the decimal number
 *   $match[4] if present, is the integer (when no fraction is matched).
 *
 * @return string
 *   String representing a numerical value that will sort numerically in an
 *   alphanumeric search.
 */
function _views_natural_sort_number_transform_match_callback(array $match) {

  // Remove commas and leading zeros from whole number.
  $whole = (string) (int) str_replace(',', '', (isset($match[4]) && strlen($match[4]) > 0) ? $match[4] : $match[2]);
  // Remove traililng 0's from fraction, then add the decimal and one trailing
  // 0 and a space. The space serves as a way to always sort shorter decimal
  // numbers that match exactly as less than longer ones.
  // Ex: 3.05 and 3.05011.
  $fraction = trim('.' . $match[3], '0') . '0 ';
  $encode = sprintf('%02u', strlen($whole)) . $whole . $fraction;
  if (strlen($match[1])) {
    // Negative number. Make 10's complement. Put back any leading white space
    // and the dash requires intermediate to avoid double-replacing the same
    // digit. str_replace() seems to work by copying the source to the result,
    // then successively replacing within it, rather than replacing from the
    // source to the result.
    // In this case since rules are reverced we also have to use a character
    // that would be sorted higher than a space when a number is being compared
    // against a longer one that is identical in negative numbers. This is so
    // that longer numbers are always LESS than sorter numbers that have
    // identical beginnings. Ex: -3.05 and -3.05011.
    $digits       = array('0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ' ');
    $intermediate = array('a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k');
    $rev_digits   = array('9', '8', '7', '6', '5', '4', '3', '2', '1', '0', ':');
    $encode       = $match[1] . str_replace($intermediate, $rev_digits, str_replace($digits, $intermediate, $encode));
  }
  return $encode;
}

/**
 * Simply convert days of the week over to numbers.
 *
 * This "should" be multilingual safe.
 *
 * @param string $string
 *   The string we are transforming days to numbers in.
 *
 * @return string
 *   The transformed string.
 */
function views_natural_sort_days_of_the_week_sort_days($string) {

  global $language;

  // Adding a configuration so that in the future an admin page can be made
  // to switch which day is the first day of the week based on local.
  $used_language = isset($language->language) ? $language->language : 'en';
  $first_day = config_get('views_natural_sort.settings', 'views_natural_sort_days_of_the_week_first_day_' . $used_language);

  $day_list = views_natural_sort_days_of_the_week_get_default_days();
  $sorted_days = $day_list;
  // Go through list and resort it and Translate it.
  $start = array_search($first_day, $day_list);
  for ($i = 0; $i < 7; $i++) {
    $current_day = ($i + $start) % 7;
    $abbreviations = views_natural_sort_days_of_the_week_get_acceptable_day_abbreviations($day_list[$current_day], $used_language);
    $translated_day = t('@day', array('@day' => $day_list[$current_day]));
    $string = preg_replace(
      array(
      '/\b' . $translated_day . '\b/i',
      '/\b(' . implode('\.?|', views_natural_sort_days_of_the_week_get_acceptable_day_abbreviations($day_list[$current_day])) . ')\b/i',
    ),
      ' ' . $i . ' ',
      $string
    );
  }
  return $string;
}

/**
 * The default days of the week.
 */
function views_natural_sort_days_of_the_week_get_default_days() {
  return array(
    "Sunday",
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
  );
}

/**
 * Gets all the configured abbreviations for a certain day of the week.
 *
 * @param string $day
 *   The day of the week we want to get abbreviations for.
 * @param string $lang
 *   Determines which configuration to search for as they are keyed by language.
 *
 * @return array
 *   The abbreviations for the day of the week and language passed in.
 */
function views_natural_sort_days_of_the_week_get_acceptable_day_abbreviations($day, $lang = 'en') {
  $default_days = views_natural_sort_days_of_the_week_get_default_days();
  $index = array_search($day, $default_days);
  $default_abbrev = views_natural_sort_days_of_the_week_get_default_day_abbreviations();
  return config_get('views_natural_sort.settings',
    'views_natural_sort_days_of_the_week_' . $lang . '_' . $day);
}

/**
 * Default mappings of days of the week abbreviationas.
 */
function views_natural_sort_days_of_the_week_get_default_day_abbreviations() {
  return array(
    array("Sun"),
    array("Mon"),
    array("Tu", "Tue", "Tues"),
    array("Wed"),
    array("Th", "Thu", "Thur", "Thurs"),
    array("Fri"),
    array("Sat"),
  );
}
