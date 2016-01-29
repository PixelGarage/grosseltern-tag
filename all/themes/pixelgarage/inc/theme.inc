<?php
/**
 * Created by PhpStorm.
 * User: ralph
 * Date: 06.02.15
 * Time: 17:22
 */

/* =============================================================================
 *      Theme language block
 * ========================================================================== */

function pixelgarage_links__locale_block(&$vars) {
  foreach($vars['links'] as $language => $langInfo) {
    $abbr = $langInfo['language']->language[0];
    $vars['links'][$language]['title'] = $abbr;
  }
  $content = theme_links($vars);
  return $content;
}

/**
 * Themes a select element as a set of checkboxes.
 *
 * @see http://api.drupal.org/api/function/theme_select/7
 *
 * @param array $vars
 *   An array of arrays, the 'element' item holds the properties of the element.
 *
 * @return string
 *   HTML representing the form element.
 */
function pixelgarage_select_as_checkboxes($vars) {
  $element = $vars['element'];

  // The selected keys from #options.
  $selected_options = empty($element['#value']) ? (empty($element['#default_value']) ? array() : $element['#default_value']) : $element['#value'];
  if (!is_array($selected_options)) {
    $selected_options = array($selected_options);
  }

  // Grab exposed filter description.  We'll put it under the label where it
  // makes more sense.
  $description = '';
  if (!empty($element['#bef_description'])) {
    $description = '<div class="description">' . $element['#bef_description'] . '</div>';
  }

  $output = '<div class="pxl-checkboxes">';
  foreach ($element['#options'] as $key => $short) {
    if ('All' === $key) {
      // TODO: 'All' text is customizable in Views.
      // No need for an 'All' option -- either unchecking or checking all the
      // checkboxes is equivalent.
      continue;
    }

    $isSelected = array_search($key, $selected_options) !== FALSE;

    $output .= '<div class="pxl-checkbox ' . $short . ($isSelected ? ' selected">' : '">');
    $output .= _pxl_checkbox($element, $key, $short, $isSelected);
    $output .= '</div>';

  }
  $output .= '</div>';

  // Fake theme_checkboxes() which we can't call because it calls
  // theme_form_element() for each option.
  $attributes['class'] = array('form-checkboxes', 'pxl-select-as-checkboxes');

  return '<div' . drupal_attributes($attributes) . ">$description$output</div>";
}

function _pxl_checkbox($element, $value, $label, $selected) {
  $value = check_plain($value);
  $label = filter_xss_admin($label);
  $id = drupal_html_id($element['#id'] . '-' . $value);
  $term = taxonomy_term_load($value);
  $path = drupal_get_path('theme', 'pixelgarage') . '/images/' . strtolower($term->field_shortcut[LANGUAGE_NONE][0]['value']) . '.svg';
  if (!file_exists($path)) {
    $path = drupal_get_path('theme', 'pixelgarage') . '/images/other.svg';
  }
  $img= '<img src="' . file_create_url($path) . '">';

  // Custom ID for each checkbox based on the <select>'s original ID.
  $properties = array(
    '#required' => FALSE,
    '#id' => $id,
    '#type' => 'bef-checkbox',
    '#name' => $id,
  );

  // Prevent the select-all-none class from cascading to all checkboxes.
  if (!empty($element['#attributes']['class'])
    && FALSE !== ($key = array_search('bef-select-all-none', $element['#attributes']['class']))) {
    unset($element['#attributes']['class'][$key]);
  }

  // Unset the name attribute as we are setting it manually.
  unset($element['#attributes']['name']);

  // Unset the multiple attribute as it doesn't apply for checkboxes.
  unset ($element['#attributes']['multiple']);

  $checkbox = '<input type="checkbox" '
    // Brackets are key -- just like select.
    . 'name="' . $element['#name'] . '[]" '
    . 'id="' . $id . '" '
    . 'value="' . $value . '" '
    . ($selected ? 'checked="checked" ' : '')
    . drupal_attributes($element['#attributes']) . ' />';
  $properties['#children'] = "$checkbox <label class='option' for='$id'>$img<div>$label</div></label>";
  $output = theme('form_element', array('element' => $properties));
  return $output;
}

