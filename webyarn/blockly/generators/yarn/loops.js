/**
 * @license
 * Copyright 2012 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @fileoverview Generating Yarn for loop blocks.
 * @author fraser@google.com (Neil Fraser)
 */
'use strict';

goog.provide('Blockly.Yarn.loops');

goog.require('Blockly.Yarn');


Blockly.Yarn['controls_repeat_ext'] = function(block) {
  // Repeat n times.
  if (block.getField('TIMES')) {
    // Internal number.
    var repeats = String(Number(block.getFieldValue('TIMES')));
  } else {
    // External number.
    var repeats = Blockly.Yarn.valueToCode(block, 'TIMES',
        Blockly.Yarn.ORDER_ASSIGNMENT) || '0';
  }
  var branch = Blockly.Yarn.statementToCode(block, 'DO');
  branch = Blockly.Yarn.addLoopTrap(branch, block);
  var code = '';
  var loopVar = Blockly.Yarn.variableDB_.getDistinctName(
      'count', Blockly.VARIABLE_CATEGORY_NAME);
  var endVar = repeats;
  if (!repeats.match(/^\w+$/) && !Blockly.isNumber(repeats)) {
    endVar = Blockly.Yarn.variableDB_.getDistinctName(
        'repeat_end', Blockly.VARIABLE_CATEGORY_NAME);
    code += 'var ' + endVar + ' = ' + repeats + ';\n';
  }
  code += 'for (var ' + loopVar + ' = 0; ' +
      loopVar + ' < ' + endVar + '; ' +
      loopVar + '++) {\n' +
      branch + '}\n';
  return code;
};

Blockly.Yarn['controls_repeat'] =
    Blockly.Yarn['controls_repeat_ext'];

Blockly.Yarn['controls_while'] = function(block) {
  // while loop.
  var argument0 = Blockly.Yarn.valueToCode(block, 'BOOL',      
      Blockly.Yarn.ORDER_NONE) || 'false';
  var branch = Blockly.Yarn.statementToCode(block, 'DO');
  branch = Blockly.Yarn.addLoopTrap(branch, block);
  return '(while ' + argument0 + '\n' + branch + ')\n';
};


Blockly.Yarn['controls_whileUntil'] = function(block) {
  // Do while/until loop.
  var until = block.getFieldValue('MODE') == 'UNTIL';
  var argument0 = Blockly.Yarn.valueToCode(block, 'BOOL',
      until ? Blockly.Yarn.ORDER_LOGICAL_NOT :
      Blockly.Yarn.ORDER_NONE) || 'false';
  var branch = Blockly.Yarn.statementToCode(block, 'DO');
  branch = Blockly.Yarn.addLoopTrap(branch, block);
  if (until) {
    argument0 = '!' + argument0;
  }
  return '(while ' + argument0 + '\n' + branch + ')\n';
};

Blockly.Yarn['controls_for'] = function(block) {
  // For loop.
  var variable0 = Blockly.Yarn.variableDB_.getName(
      block.getFieldValue('VAR'), Blockly.VARIABLE_CATEGORY_NAME);
  var argument0 = Blockly.Yarn.valueToCode(block, 'FROM',
      Blockly.Yarn.ORDER_ASSIGNMENT) || '0';
  var argument1 = Blockly.Yarn.valueToCode(block, 'TO',
      Blockly.Yarn.ORDER_ASSIGNMENT) || '0';
  var increment = Blockly.Yarn.valueToCode(block, 'BY',
      Blockly.Yarn.ORDER_ASSIGNMENT) || '1';
  var branch = Blockly.Yarn.statementToCode(block, 'DO');
  branch = Blockly.Yarn.addLoopTrap(branch, block);
  var code;
  if (Blockly.isNumber(argument0) && Blockly.isNumber(argument1) &&
      Blockly.isNumber(increment)) {
    // All arguments are simple numbers.
    var up = Number(argument0) <= Number(argument1);
    code = 'for (' + variable0 + ' = ' + argument0 + '; ' +
        variable0 + (up ? ' <= ' : ' >= ') + argument1 + '; ' +
        variable0;
    var step = Math.abs(Number(increment));
    if (step == 1) {
      code += up ? '++' : '--';
    } else {
      code += (up ? ' += ' : ' -= ') + step;
    }
    code += ') {\n' + branch + '}\n';
  } else {
    code = '';
    // Cache non-trivial values to variables to prevent repeated look-ups.
    var startVar = argument0;
    if (!argument0.match(/^\w+$/) && !Blockly.isNumber(argument0)) {
      startVar = Blockly.Yarn.variableDB_.getDistinctName(
          variable0 + '_start', Blockly.VARIABLE_CATEGORY_NAME);
      code += 'var ' + startVar + ' = ' + argument0 + ';\n';
    }
    var endVar = argument1;
    if (!argument1.match(/^\w+$/) && !Blockly.isNumber(argument1)) {
      endVar = Blockly.Yarn.variableDB_.getDistinctName(
          variable0 + '_end', Blockly.VARIABLE_CATEGORY_NAME);
      code += 'var ' + endVar + ' = ' + argument1 + ';\n';
    }
    // Determine loop direction at start, in case one of the bounds
    // changes during loop execution.
    var incVar = Blockly.Yarn.variableDB_.getDistinctName(
        variable0 + '_inc', Blockly.VARIABLE_CATEGORY_NAME);
    code += 'var ' + incVar + ' = ';
    if (Blockly.isNumber(increment)) {
      code += Math.abs(increment) + ';\n';
    } else {
      code += 'Math.abs(' + increment + ');\n';
    }
    code += 'if (' + startVar + ' > ' + endVar + ') {\n';
    code += Blockly.Yarn.INDENT + incVar + ' = -' + incVar + ';\n';
    code += '}\n';
    code += 'for (' + variable0 + ' = ' + startVar + '; ' +
        incVar + ' >= 0 ? ' +
        variable0 + ' <= ' + endVar + ' : ' +
        variable0 + ' >= ' + endVar + '; ' +
        variable0 + ' += ' + incVar + ') {\n' +
        branch + '}\n';
  }
  return code;
};

Blockly.Yarn['controls_forEach'] = function(block) {
  // For each loop.
  var variable0 = Blockly.Yarn.variableDB_.getName(
      block.getFieldValue('VAR'), Blockly.VARIABLE_CATEGORY_NAME);
  var argument0 = Blockly.Yarn.valueToCode(block, 'LIST',
      Blockly.Yarn.ORDER_ASSIGNMENT) || '[]';
  var branch = Blockly.Yarn.statementToCode(block, 'DO');
  branch = Blockly.Yarn.addLoopTrap(branch, block);
  var code = '';
  // Cache non-trivial values to variables to prevent repeated look-ups.
  var listVar = argument0;
  if (!argument0.match(/^\w+$/)) {
    listVar = Blockly.Yarn.variableDB_.getDistinctName(
        variable0 + '_list', Blockly.VARIABLE_CATEGORY_NAME);
    code += 'var ' + listVar + ' = ' + argument0 + ';\n';
  }
  var indexVar = Blockly.Yarn.variableDB_.getDistinctName(
      variable0 + '_index', Blockly.VARIABLE_CATEGORY_NAME);
  branch = Blockly.Yarn.INDENT + variable0 + ' = ' +
      listVar + '[' + indexVar + '];\n' + branch;
  code += 'for (var ' + indexVar + ' in ' + listVar + ') {\n' + branch + '}\n';
  return code;
};

Blockly.Yarn['controls_flow_statements'] = function(block) {
  // Flow statements: continue, break.
  var xfix = '';
  if (Blockly.Yarn.STATEMENT_PREFIX) {
    // Automatic prefix insertion is switched off for this block.  Add manually.
    xfix += Blockly.Yarn.injectId(Blockly.Yarn.STATEMENT_PREFIX,
        block);
  }
  if (Blockly.Yarn.STATEMENT_SUFFIX) {
    // Inject any statement suffix here since the regular one at the end
    // will not get executed if the break/continue is triggered.
    xfix += Blockly.Yarn.injectId(Blockly.Yarn.STATEMENT_SUFFIX,
        block);
  }
  if (Blockly.Yarn.STATEMENT_PREFIX) {
    var loop = Blockly.Constants.Loops
        .CONTROL_FLOW_IN_LOOP_CHECK_MIXIN.getSurroundLoop(block);
    if (loop && !loop.suppressPrefixSuffix) {
      // Inject loop's statement prefix here since the regular one at the end
      // of the loop will not get executed if 'continue' is triggered.
      // In the case of 'break', a prefix is needed due to the loop's suffix.
      xfix += Blockly.Yarn.injectId(Blockly.Yarn.STATEMENT_PREFIX,
          loop);
    }
  }
  switch (block.getFieldValue('FLOW')) {
    case 'BREAK':
      return xfix + 'break;\n';
    case 'CONTINUE':
      return xfix + 'continue;\n';
  }
  throw Error('Unknown flow statement.');
};
