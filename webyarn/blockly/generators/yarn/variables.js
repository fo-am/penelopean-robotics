/**
 * @license
 * Copyright 2012 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @fileoverview Generating Yarn for variable blocks.
 * @author fraser@google.com (Neil Fraser)
 */
'use strict';

goog.provide('Blockly.Yarn.variables');

goog.require('Blockly.Yarn');


Blockly.Yarn['variables_get'] = function(block) {
  // Variable getter.
  var code = Blockly.Yarn.variableDB_.getName(block.getFieldValue('VAR'),
      Blockly.VARIABLE_CATEGORY_NAME);
  return [code, Blockly.Yarn.ORDER_ATOMIC];
};

Blockly.Yarn['variables_set'] = function(block) {
  // Variable setter.
  var argument0 = Blockly.Yarn.valueToCode(block, 'VALUE',
      Blockly.Yarn.ORDER_ASSIGNMENT) || '0';
  var varName = Blockly.Yarn.variableDB_.getName(
      block.getFieldValue('VAR'), Blockly.VARIABLE_CATEGORY_NAME);
  return '(set! '+varName + ' ' + argument0 + ')\n';
};
