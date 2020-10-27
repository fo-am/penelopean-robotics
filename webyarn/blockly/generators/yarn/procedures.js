/**
 * @license
 * Copyright 2012 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @fileoverview Generating Yarn for procedure blocks.
 * @author fraser@google.com (Neil Fraser)
 */
'use strict';

goog.provide('Blockly.Yarn.procedures');

goog.require('Blockly.Yarn');


Blockly.Yarn['procedures_defreturn'] = function(block) {
  // Define a procedure with a return value.
  var funcName = Blockly.Yarn.variableDB_.getName(
      block.getFieldValue('NAME'), Blockly.PROCEDURE_CATEGORY_NAME);
  var xfix1 = '';
  if (Blockly.Yarn.STATEMENT_PREFIX) {
    xfix1 += Blockly.Yarn.injectId(Blockly.Yarn.STATEMENT_PREFIX,
        block);
  }
  if (Blockly.Yarn.STATEMENT_SUFFIX) {
    xfix1 += Blockly.Yarn.injectId(Blockly.Yarn.STATEMENT_SUFFIX,
        block);
  }
  if (xfix1) {
    xfix1 = Blockly.Yarn.prefixLines(xfix1, Blockly.Yarn.INDENT);
  }
  var loopTrap = '';
  if (Blockly.Yarn.INFINITE_LOOP_TRAP) {
    loopTrap = Blockly.Yarn.prefixLines(
        Blockly.Yarn.injectId(Blockly.Yarn.INFINITE_LOOP_TRAP,
        block), Blockly.Yarn.INDENT);
  }
  var branch = Blockly.Yarn.statementToCode(block, 'STACK');
  var returnValue = Blockly.Yarn.valueToCode(block, 'RETURN',
      Blockly.Yarn.ORDER_NONE) || '';
  var xfix2 = '';
  if (branch && returnValue) {
    // After executing the function body, revisit this block for the return.
    xfix2 = xfix1;
  }
  if (returnValue) {
    returnValue = Blockly.Yarn.INDENT + ' ' + returnValue + ';\n';
  }
  var args = [];
  var variables = block.getVars();
  /*for (var i = 0; i < variables.length; i++) {
    args[i] = Blockly.Yarn.variableDB_.getName(variables[i],
        Blockly.VARIABLE_CATEGORY_NAME);
  }*/
  var code = '(defun (' + funcName + ' ' + args.join(', ') + ')\n' +
      xfix1 + loopTrap + branch + xfix2 + returnValue + ')';
  code = Blockly.Yarn.scrub_(block, code);
  // Add % so as not to collide with helper functions in definitions list.
  Blockly.Yarn.definitions_['%' + funcName] = code;
  return null;
};

// Defining a procedure without a return value uses the same generator as
// a procedure with a return value.
Blockly.Yarn['procedures_defnoreturn'] =
    Blockly.Yarn['procedures_defreturn'];

Blockly.Yarn['procedures_callreturn'] = function(block) {
  // Call a procedure with a return value.
  var funcName = Blockly.Yarn.variableDB_.getName(
      block.getFieldValue('NAME'), Blockly.PROCEDURE_CATEGORY_NAME);
  var args = [];
  var variables = block.getVars();
  for (var i = 0; i < variables.length; i++) {
    args[i] = Blockly.Yarn.valueToCode(block, 'ARG' + i,
        Blockly.Yarn.ORDER_COMMA) || 'null';
  }
  var code = '(' + funcName + ' ' + args.join(' ') + ')';
  return [code, Blockly.Yarn.ORDER_FUNCTION_CALL];
};

Blockly.Yarn['procedures_callnoreturn'] = function(block) {
  // Call a procedure with no return value.
  // Generated code is for a function call as a statement is the same as a
  // function call as a value, with the addition of line ending.
  var tuple = Blockly.Yarn['procedures_callreturn'](block);
  return tuple[0] + ';\n';
};

Blockly.Yarn['procedures_ifreturn'] = function(block) {
  // Conditionally return value from a procedure.
  var condition = Blockly.Yarn.valueToCode(block, 'CONDITION',
      Blockly.Yarn.ORDER_NONE) || 'false';
  var code = '(when ' + condition + ' \n';
  if (Blockly.Yarn.STATEMENT_SUFFIX) {
    // Inject any statement suffix here since the regular one at the end
    // will not get executed if the return is triggered.
    code += Blockly.Yarn.prefixLines(
        Blockly.Yarn.injectId(Blockly.Yarn.STATEMENT_SUFFIX, block),
        Blockly.Yarn.INDENT);
  }
  if (block.hasReturnValue_) {
    var value = Blockly.Yarn.valueToCode(block, 'VALUE',
        Blockly.Yarn.ORDER_NONE) || 'null';
    code += Blockly.Yarn.INDENT + ' ' + value + ';\n';
  } else {
    code += Blockly.Yarn.INDENT + '\n';
  }
  code += ')\n';
  return code;
};
