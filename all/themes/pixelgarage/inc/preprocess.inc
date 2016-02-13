<?php
/**
 * Created by PhpStorm.
 * User: ralph
 * Date: 06.02.15
 * Time: 17:23
 */

/**
 * Preprocess variables for the html template.
 */
function pixelgarage_preprocess_html(&$vars) {
  // make sure jQuery UI and effects is loaded for anonymous users
  drupal_add_library('system', 'ui');
  drupal_add_library('system', 'effects');

}

/**
 * Override or insert variables for the page templates.
 */
function pixelgarage_preprocess_page(&$vars) {
  // hide titles on login forms
  pg_login_preprocess_page($vars);

  // replace logo with .svg
  $vars['logo'] = str_replace('.jpg', '.svg', $vars['logo']);
}

/**
 * Implements template_preprocess_views_exposed_form().
 *
 * Adds total result count to exposed form and the placeholder for the search field.
 */
function pixelgarage_preprocess_views_exposed_form(&$vars) {
  $view = views_get_page_view();
  $vars['total_rows'] = null;
  if ($filter = &$vars['widgets']['filter-combine']) {
    $placeholder = 'placeholder="' . $filter->label . '"/>';
    $filter->widget = str_replace('/>', $placeholder, $filter->widget);
  }

  if (empty($vars['form']['combine']['#value'])) return;

  $page_total = count($view->result);
  if(isset($view->total_rows)){
    $text = ($view->total_rows == 1) ? '%hits hit' : '%hits hits';
    $vars['total_rows'] = t($text, array('%hits' => $view->total_rows));
  } else {
    $text = ($page_total == 1) ? '%hits hit' : '%hits hits';
    $vars['total_rows'] = t($text, array('%hits' => $page_total));
  }
}

/**
 * Implements template_preprocess_webform_confirmation().
 */
function pixelgarage_preprocess_webform_confirmation(&$vars) {
  // because we have translated webform nodes, we have to get the submissions
  // of the original node of the translation set, set in the "tnid" property
  if (!isset($vars['node']) || !isset($vars['sid'])) return;

  $webform = $vars['node'];
  $tnid = $webform->tnid ? $webform->tnid : $webform->nid;
  $orig_form = node_load($tnid);

  $submissions = webform_get_submissions(array('nid' => $tnid, 'sid' => $vars['sid']));
  $submission = $submissions[$vars['sid']];

  // create a new posting node
  $status = (!empty($orig_form->field_publish_post_immediately) && $orig_form->field_publish_post_immediately[LANGUAGE_NONE][0]['value'] == 1) ? 1 : 0;
  $post = new stdClass();
  $post->type = 'post';
  $post->language = LANGUAGE_NONE;
  $post->uid = 1;
  $post->status = $status;
  $post->comment = 0;
  $post->promote = 0;
  node_object_prepare($post);

  // fill the node properties and save it to the database
  $post->title = $submission->data[4][0];
  $post->field_e_mail[$post->language][0]['email'] = $submission->data[2][0]; // email
  $post->field_name[$post->language][0]['value'] = $submission->data[4][0]; // Name(s)
  $post->field_image[$post->language][0]['fid'] = $submission->data[1][0]; // fid
  $post->field_quote[$post->language][0]['value'] = substr($submission->data[5][0], 0, 70); // Quote
  $post->field_thanks[$post->language][0]['value'] = substr($submission->data[11][0], 0, 20); // Thanks

  $post = node_submit($post);
  node_save($post);

  // get node and render it
  $post = node_load($post->nid);
  $vars['post'] = node_view($post, 'full');
}

