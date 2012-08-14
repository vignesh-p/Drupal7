<?php

/**
 * @file
 * Page callback file for the shopmenu module.
 */

/**
 * Menu callback; displays a Drupal page containing recent shopmenu entries of a given user.
 */
function shopmenu_page_user($account) {
  global $user;

  drupal_set_title($title = t("@name's shopmenu", array('@name' => format_username($account))), PASS_THROUGH);

  $build = array();

  $query = db_select('node', 'n')->extend('PagerDefault');
  $nids = $query
    ->fields('n', array('nid', 'sticky', 'created'))
    ->condition('type', 'shopmenu')
    ->condition('uid', $account->uid)
    ->condition('status', 1)
    ->orderBy('sticky', 'DESC')
    ->orderBy('created', 'DESC')
    ->limit(variable_get('default_nodes_main', 10))
    ->addTag('node_access')
    ->execute()
    ->fetchCol();

  if (!empty($nids)) {
    $nodes = node_load_multiple($nids);
    $build += node_view_multiple($nodes);
    $build['pager'] = array(
      '#theme' => 'pager',
      '#weight' => 5,
    );
  }
  else {
    if ($account->uid == $user->uid) {
      drupal_set_message(t('You have not created any shopmenu entries.'));
    }
    else {
      drupal_set_message(t('!author has not created any shopmenu entries.', array('!author' => theme('username', array('account' => $account)))));
    }
  }
  drupal_add_feed('shopmenu/' . $account->uid . '/feed', t('RSS - !title', array('!title' => $title)));

  return $build;
}

/**
 * Menu callback; displays a Drupal page containing recent shopmenu entries of all users.
 */
function shopmenu_page_last() {
  global $user;
  $build = array();

  $query = db_select('node', 'n')->extend('PagerDefault');
  $nids = $query
    ->fields('n', array('nid', 'sticky', 'created'))
    ->condition('type', 'shopmenu')
    ->condition('status', 1)
    ->orderBy('sticky', 'DESC')
    ->orderBy('created', 'DESC')
    ->limit(variable_get('default_nodes_main', 10))
    ->addTag('node_access')
    ->execute()
    ->fetchCol();

  if (!empty($nids)) {
    $nodes = node_load_multiple($nids);
    $build += node_view_multiple($nodes);
    $build['pager'] = array(
      '#theme' => 'pager',
      '#weight' => 5,
    );
  }
  else {
    drupal_set_message(t('No shopmenu entries have been created.'));
  }
  drupal_add_feed('shopmenu/feed', t('RSS - shopmenus'));

  return $build;
}

/**
 * Menu callback; displays an RSS feed containing recent shopmenu entries of a given user.
 */
function shopmenu_feed_user($account) {

  $nids = db_select('node', 'n')
    ->fields('n', array('nid', 'created'))
    ->condition('type', 'shopmenu')
    ->condition('uid', $account->uid)
    ->condition('status', 1)
    ->orderBy('created', 'DESC')
    ->range(0, variable_get('feed_default_items', 10))
    ->addTag('node_access')
    ->execute()
    ->fetchCol();

  $channel['title'] = t("!name's shopmenu", array('!name' => format_username($account)));
  $channel['link'] = url('shopmenu/' . $account->uid, array('absolute' => TRUE));

  node_feed($nids, $channel);
}

/**
 * Menu callback; displays an RSS feed containing recent shopmenu entries of all users.
 */
function shopmenu_feed_last() {
  $nids = db_select('node', 'n')
    ->fields('n', array('nid', 'created'))
    ->condition('type', 'shopmenu')
    ->condition('status', 1)
    ->orderBy('created', 'DESC')
    ->range(0, variable_get('feed_default_items', 10))
    ->addTag('node_access')
    ->execute()
    ->fetchCol();

  $channel['title'] = t('!site_name shopmenus', array('!site_name' => variable_get('site_name', 'Drupal')));
  $channel['link'] = url('shopmenu', array('absolute' => TRUE));

  node_feed($nids, $channel);
}
