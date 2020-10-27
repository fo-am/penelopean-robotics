/**
 * @license
 * Copyright 2012 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @fileoverview Generating Yarn for logic blocks.
 * @author q.neutron@gmail.com (Quynh Neutron)
 */
'use strict';

goog.provide('Blockly.Yarn.logic');

goog.require('Blockly.Yarn');


Blockly.Yarn['controls_if'] = function(block) {
  // If/elseif/else condition.
  var n = 0;
  var code = '', branchCode, conditionCode, elseCode;
  if (Blockly.Yarn.STATEMENT_PREFIX) {
    // Automatic prefix insertion is switched off for this block.  Add manually.
    code += Blockly.Yarn.injectId(Blockly.Yarn.STATEMENT_PREFIX,
        block);
  }
  do {
    conditionCode = Blockly.Yarn.valueToCode(block, 'IF' + n,
        Blockly.Yarn.ORDER_NONE) || 'false';
    branchCode = Blockly.Yarn.statementToCode(block, 'DO' + n);
    if (Blockly.Yarn.STATEMENT_SUFFIX) {
      branchCode = Blockly.Yarn.prefixLines(
          Blockly.Yarn.injectId(Blockly.Yarn.STATEMENT_SUFFIX,
          block), Blockly.Yarn.INDENT) + branchCode;
    }
    code += (n > 0 ? ' else ' : '') +
        '(when ' + conditionCode + '\n' + branchCode + ')';
    ++n;
  } while (block.getInput('IF' + n));

  if (block.getInput('ELSE') || Blockly.Yarn.STATEMENT_SUFFIX) {
    elseCode = Blockly.Yarn.statementToCode(block, 'ELSE');
    if (Blockly.Yarn.STATEMENT_SUFFIX) {
      elseCode = Blockly.Yarn.prefixLines(
          Blockly.Yarn.injectId(Blockly.Yarn.STATEMENT_SUFFIX,
          block), Blockly.Yarn.INDENT) + elseCode;
    }
    code = '(if ' + conditionCode + "\n" + branchCode + "\n" + elseCode + ')';
  }
  return code + '\n';
};

Blockly.Yarn['controls_ifelse'] = Blockly.Yarn['controls_if'];

Blockly.Yarn['logic_compare'] = function(block) {
  // Comparison operator.
  var OPERATORS = {
    'EQ': '==',
    'NEQ': '!=',
    'LT': '<',
    'LTE': '<=',
    'GT': '>',
    'GTE': '>='
  };
  var operator = OPERATORS[block.getFieldValue('OP')];
  var order = (operator == '==' || operator == '!=') ?
      Blockly.Yarn.ORDER_EQUALITY : Blockly.Yarn.ORDER_RELATIONAL;
  var argument0 = Blockly.Yarn.valueToCode(block, 'A', order) || '0';
  var argument1 = Blockly.Yarn.valueToCode(block, 'B', order) || '0';
  var code = argument0 + ' ' + operator + ' ' + argument1;
  return [code, order];
};

Blockly.Yarn['logic_operation'] = function(block) {
  // Operations 'and', 'or'.
  var operator = (block.getFieldValue('OP') == 'AND') ? '&&' : '||';
  var order = (operator == '&&') ? Blockly.Yarn.ORDER_LOGICAL_AND :
      Blockly.Yarn.ORDER_LOGICAL_OR;
  var argument0 = Blockly.Yarn.valueToCode(block, 'A', order);
  var argument1 = Blockly.Yarn.valueToCode(block, 'B', order);
  if (!argument0 && !argument1) {
    // If there are no arguments, then the return value is false.
    argument0 = 'false';
    argument1 = 'false';
  } else {
    // Single missing arguments have no effect on the return value.
    var defaultArgument = (operator == '&&') ? 'true' : 'false';
    if (!argument0) {
      argument0 = defaultArgument;
    }
    if (!argument1) {
      argument1 = defaultArgument;
    }
  }
  var code = argument0 + ' ' + operator + ' ' + argument1;
  return [code, order];
};

Blockly.Yarn['logic_negate'] = function(block) {
  // Negation.
  var order = Blockly.Yarn.ORDER_LOGICAL_NOT;
  var argument0 = Blockly.Yarn.valueToCode(block, 'BOOL', order) ||
      'true';
  var code = '!' + argument0;
  return [code, order];
};

Blockly.Yarn['logic_boolean'] = function(block) {
  // Boolean values true and false.
  var code = (block.getFieldValue('BOOL') == 'TRUE') ? 'true' : 'false';
  return [code, Blockly.Yarn.ORDER_ATOMIC];
};

Blockly.Yarn['logic_null'] = function(block) {
  // Null data type.
  return ['null', Blockly.Yarn.ORDER_ATOMIC];
};

Blockly.Yarn['logic_ternary'] = function(block) {
  // Ternary operator.
  var value_if = Blockly.Yarn.valueToCode(block, 'IF',
      Blockly.Yarn.ORDER_CONDITIONAL) || 'false';
  var value_then = Blockly.Yarn.valueToCode(block, 'THEN',
      Blockly.Yarn.ORDER_CONDITIONAL) || 'null';
  var value_else = Blockly.Yarn.valueToCode(block, 'ELSE',
      Blockly.Yarn.ORDER_CONDITIONAL) || 'null';
  var code = value_if + ' ? ' + value_then + ' : ' + value_else;
  return [code, Blockly.Yarn.ORDER_CONDITIONAL];
};
