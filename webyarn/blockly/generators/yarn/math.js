/**
 * @license
 * Copyright 2012 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @fileoverview Generating Yarn for math blocks.
 * @author q.neutron@gmail.com (Quynh Neutron)
 */
'use strict';

goog.provide('Blockly.Yarn.math');

goog.require('Blockly.Yarn');


Blockly.Yarn['math_number'] = function(block) {
  // Numeric value.
  var code = Number(block.getFieldValue('NUM'));
  var order = code >= 0 ? Blockly.Yarn.ORDER_ATOMIC :
              Blockly.Yarn.ORDER_UNARY_NEGATION;
  return [code, order];
};

Blockly.Yarn['math_arithmetic_yarn'] = function(block) {
  // Basic arithmetic operators, and power.
  var OPERATORS = {
    'ADD': ['+ ', Blockly.Yarn.ORDER_ADDITION],
    'MINUS': ['- ', Blockly.Yarn.ORDER_SUBTRACTION],
    'MULTIPLY': ['* ', Blockly.Yarn.ORDER_MULTIPLICATION],
    'DIVIDE': ['/ ', Blockly.Yarn.ORDER_DIVISION],
    'MOD': ['mod ', Blockly.Yarn.ORDER_DIVISION],
  };
  var tuple = OPERATORS[block.getFieldValue('OP')];
  var operator = tuple[0];
  var order = tuple[1];
  var argument0 = Blockly.Yarn.valueToCode(block, 'A', order) || '0';
  var argument1 = Blockly.Yarn.valueToCode(block, 'B', order) || '0';
  var code;
    code = '(' + operator + argument0 + ' ' + argument1 +')';
  return [code, order];
};

Blockly.Yarn['math_bitwise'] = function(block) {
  // Basic arithmetic operators, and power.
  var OPERATORS = {
    'AND': ['and ', Blockly.Yarn.ORDER_DIVISION],
    'OR': ['or ', Blockly.Yarn.ORDER_DIVISION],
    'XOR': ['xor ', Blockly.Yarn.ORDER_DIVISION],
    'BITWISE-NOT': ['bitwise-not ', Blockly.Yarn.ORDER_DIVISION],
    'NOT': ['not ', Blockly.Yarn.ORDER_DIVISION],
    'SHIFT-LEFT': ['<< ', Blockly.Yarn.ORDER_DIVISION],
    'SHIFT-RIGHT': ['>> ', Blockly.Yarn.ORDER_DIVISION],
  };
  var tuple = OPERATORS[block.getFieldValue('OP')];
  var operator = tuple[0];
  var order = tuple[1];
  var argument0 = Blockly.Yarn.valueToCode(block, 'A', order) || '0';
  var argument1 = Blockly.Yarn.valueToCode(block, 'B', order) || '0';
  var code;
    code = '(' + operator + argument0 + ' ' + argument1 +')';
  return [code, order];
};

Blockly.Yarn['math_single'] = function(block) {
  // Math operators with single operand.
  var operator = block.getFieldValue('OP');
  var code;
  var arg;
  if (operator == 'NEG') {
    // Negation is a special case given its different operator precedence.
    arg = Blockly.Yarn.valueToCode(block, 'NUM',
        Blockly.Yarn.ORDER_UNARY_NEGATION) || '0';
    if (arg[0] == '-') {
      // --3 is not legal in JS.
      arg = ' ' + arg;
    }
    code = '-' + arg;
    return [code, Blockly.Yarn.ORDER_UNARY_NEGATION];
  }
  if (operator == 'SIN' || operator == 'COS' || operator == 'TAN') {
    arg = Blockly.Yarn.valueToCode(block, 'NUM',
        Blockly.Yarn.ORDER_DIVISION) || '0';
  } else {
    arg = Blockly.Yarn.valueToCode(block, 'NUM',
        Blockly.Yarn.ORDER_NONE) || '0';
  }
  // First, handle cases which generate values that don't need parentheses
  // wrapping the code.
  switch (operator) {
    case 'ABS':
      code = 'Math.abs(' + arg + ')';
      break;
    case 'ROOT':
      code = 'Math.sqrt(' + arg + ')';
      break;
    case 'LN':
      code = 'Math.log(' + arg + ')';
      break;
    case 'EXP':
      code = 'Math.exp(' + arg + ')';
      break;
    case 'POW10':
      code = 'Math.pow(10,' + arg + ')';
      break;
    case 'ROUND':
      code = 'Math.round(' + arg + ')';
      break;
    case 'ROUNDUP':
      code = 'Math.ceil(' + arg + ')';
      break;
    case 'ROUNDDOWN':
      code = 'Math.floor(' + arg + ')';
      break;
    case 'SIN':
      code = 'Math.sin(' + arg + ' / 180 * Math.PI)';
      break;
    case 'COS':
      code = 'Math.cos(' + arg + ' / 180 * Math.PI)';
      break;
    case 'TAN':
      code = 'Math.tan(' + arg + ' / 180 * Math.PI)';
      break;
  }
  if (code) {
    return [code, Blockly.Yarn.ORDER_FUNCTION_CALL];
  }
  // Second, handle cases which generate values that may need parentheses
  // wrapping the code.
  switch (operator) {
    case 'LOG10':
      code = 'Math.log(' + arg + ') / Math.log(10)';
      break;
    case 'ASIN':
      code = 'Math.asin(' + arg + ') / Math.PI * 180';
      break;
    case 'ACOS':
      code = 'Math.acos(' + arg + ') / Math.PI * 180';
      break;
    case 'ATAN':
      code = 'Math.atan(' + arg + ') / Math.PI * 180';
      break;
    default:
      throw Error('Unknown math operator: ' + operator);
  }
  return [code, Blockly.Yarn.ORDER_DIVISION];
};

Blockly.Yarn['math_constant'] = function(block) {
  // Constants: PI, E, the Golden Ratio, sqrt(2), 1/sqrt(2), INFINITY.
  var CONSTANTS = {
    'PI': ['Math.PI', Blockly.Yarn.ORDER_MEMBER],
    'E': ['Math.E', Blockly.Yarn.ORDER_MEMBER],
    'GOLDEN_RATIO':
        ['(1 + Math.sqrt(5)) / 2', Blockly.Yarn.ORDER_DIVISION],
    'SQRT2': ['Math.SQRT2', Blockly.Yarn.ORDER_MEMBER],
    'SQRT1_2': ['Math.SQRT1_2', Blockly.Yarn.ORDER_MEMBER],
    'INFINITY': ['Infinity', Blockly.Yarn.ORDER_ATOMIC]
  };
  return CONSTANTS[block.getFieldValue('CONSTANT')];
};

Blockly.Yarn['math_number_property'] = function(block) {
  // Check if a number is even, odd, prime, whole, positive, or negative
  // or if it is divisible by certain number. Returns true or false.
  var number_to_check = Blockly.Yarn.valueToCode(block, 'NUMBER_TO_CHECK',
      Blockly.Yarn.ORDER_MODULUS) || '0';
  var dropdown_property = block.getFieldValue('PROPERTY');
  var code;
  if (dropdown_property == 'PRIME') {
    // Prime is a special case as it is not a one-liner test.
    var functionName = Blockly.Yarn.provideFunction_(
        'mathIsPrime',
        ['function ' + Blockly.Yarn.FUNCTION_NAME_PLACEHOLDER_ + '(n) {',
         '  // https://en.wikipedia.org/wiki/Primality_test#Naive_methods',
         '  if (n == 2 || n == 3) {',
         '    return true;',
         '  }',
         '  // False if n is NaN, negative, is 1, or not whole.',
         '  // And false if n is divisible by 2 or 3.',
         '  if (isNaN(n) || n <= 1 || n % 1 != 0 || n % 2 == 0 ||' +
            ' n % 3 == 0) {',
         '    return false;',
         '  }',
         '  // Check all the numbers of form 6k +/- 1, up to sqrt(n).',
         '  for (var x = 6; x <= Math.sqrt(n) + 1; x += 6) {',
         '    if (n % (x - 1) == 0 || n % (x + 1) == 0) {',
         '      return false;',
         '    }',
         '  }',
         '  return true;',
         '}']);
    code = functionName + '(' + number_to_check + ')';
    return [code, Blockly.Yarn.ORDER_FUNCTION_CALL];
  }
  switch (dropdown_property) {
    case 'EVEN':
      code = number_to_check + ' % 2 == 0';
      break;
    case 'ODD':
      code = number_to_check + ' % 2 == 1';
      break;
    case 'WHOLE':
      code = number_to_check + ' % 1 == 0';
      break;
    case 'POSITIVE':
      code = number_to_check + ' > 0';
      break;
    case 'NEGATIVE':
      code = number_to_check + ' < 0';
      break;
    case 'DIVISIBLE_BY':
      var divisor = Blockly.Yarn.valueToCode(block, 'DIVISOR',
          Blockly.Yarn.ORDER_MODULUS) || '0';
      code = number_to_check + ' % ' + divisor + ' == 0';
      break;
  }
  return [code, Blockly.Yarn.ORDER_EQUALITY];
};

Blockly.Yarn['math_change'] = function(block) {
  // Add to a variable in place.
  var argument0 = Blockly.Yarn.valueToCode(block, 'DELTA',
      Blockly.Yarn.ORDER_ADDITION) || '0';
  var varName = Blockly.Yarn.variableDB_.getName(
      block.getFieldValue('VAR'), Blockly.VARIABLE_CATEGORY_NAME);
  return varName + ' = (typeof ' + varName + ' == \'number\' ? ' + varName +
      ' : 0) + ' + argument0 + ';\n';
};

// Rounding functions have a single operand.
Blockly.Yarn['math_round'] = Blockly.Yarn['math_single'];
// Trigonometry functions have a single operand.
Blockly.Yarn['math_trig'] = Blockly.Yarn['math_single'];

Blockly.Yarn['math_on_list'] = function(block) {
  // Math functions for lists.
  var func = block.getFieldValue('OP');
  var list, code;
  switch (func) {
    case 'SUM':
      list = Blockly.Yarn.valueToCode(block, 'LIST',
          Blockly.Yarn.ORDER_MEMBER) || '[]';
      code = list + '.reduce(function(x, y) {return x + y;})';
      break;
    case 'MIN':
      list = Blockly.Yarn.valueToCode(block, 'LIST',
          Blockly.Yarn.ORDER_COMMA) || '[]';
      code = 'Math.min.apply(null, ' + list + ')';
      break;
    case 'MAX':
      list = Blockly.Yarn.valueToCode(block, 'LIST',
          Blockly.Yarn.ORDER_COMMA) || '[]';
      code = 'Math.max.apply(null, ' + list + ')';
      break;
    case 'AVERAGE':
      // mathMean([null,null,1,3]) == 2.0.
      var functionName = Blockly.Yarn.provideFunction_(
          'mathMean',
          ['function ' + Blockly.Yarn.FUNCTION_NAME_PLACEHOLDER_ +
              '(myList) {',
            '  return myList.reduce(function(x, y) {return x + y;}) / ' +
                  'myList.length;',
            '}']);
      list = Blockly.Yarn.valueToCode(block, 'LIST',
          Blockly.Yarn.ORDER_NONE) || '[]';
      code = functionName + '(' + list + ')';
      break;
    case 'MEDIAN':
      // mathMedian([null,null,1,3]) == 2.0.
      var functionName = Blockly.Yarn.provideFunction_(
          'mathMedian',
          ['function ' + Blockly.Yarn.FUNCTION_NAME_PLACEHOLDER_ +
              '(myList) {',
            '  var localList = myList.filter(function (x) ' +
              '{return typeof x == \'number\';});',
            '  if (!localList.length) return null;',
            '  localList.sort(function(a, b) {return b - a;});',
            '  if (localList.length % 2 == 0) {',
            '    return (localList[localList.length / 2 - 1] + ' +
              'localList[localList.length / 2]) / 2;',
            '  } else {',
            '    return localList[(localList.length - 1) / 2];',
            '  }',
            '}']);
      list = Blockly.Yarn.valueToCode(block, 'LIST',
          Blockly.Yarn.ORDER_NONE) || '[]';
      code = functionName + '(' + list + ')';
      break;
    case 'MODE':
      // As a list of numbers can contain more than one mode,
      // the returned result is provided as an array.
      // Mode of [3, 'x', 'x', 1, 1, 2, '3'] -> ['x', 1].
      var functionName = Blockly.Yarn.provideFunction_(
          'mathModes',
          ['function ' + Blockly.Yarn.FUNCTION_NAME_PLACEHOLDER_ +
              '(values) {',
            '  var modes = [];',
            '  var counts = [];',
            '  var maxCount = 0;',
            '  for (var i = 0; i < values.length; i++) {',
            '    var value = values[i];',
            '    var found = false;',
            '    var thisCount;',
            '    for (var j = 0; j < counts.length; j++) {',
            '      if (counts[j][0] === value) {',
            '        thisCount = ++counts[j][1];',
            '        found = true;',
            '        break;',
            '      }',
            '    }',
            '    if (!found) {',
            '      counts.push([value, 1]);',
            '      thisCount = 1;',
            '    }',
            '    maxCount = Math.max(thisCount, maxCount);',
            '  }',
            '  for (var j = 0; j < counts.length; j++) {',
            '    if (counts[j][1] == maxCount) {',
            '        modes.push(counts[j][0]);',
            '    }',
            '  }',
            '  return modes;',
            '}']);
      list = Blockly.Yarn.valueToCode(block, 'LIST',
          Blockly.Yarn.ORDER_NONE) || '[]';
      code = functionName + '(' + list + ')';
      break;
    case 'STD_DEV':
      var functionName = Blockly.Yarn.provideFunction_(
          'mathStandardDeviation',
          ['function ' + Blockly.Yarn.FUNCTION_NAME_PLACEHOLDER_ +
              '(numbers) {',
            '  var n = numbers.length;',
            '  if (!n) return null;',
            '  var mean = numbers.reduce(function(x, y) {return x + y;}) / n;',
            '  var variance = 0;',
            '  for (var j = 0; j < n; j++) {',
            '    variance += Math.pow(numbers[j] - mean, 2);',
            '  }',
            '  variance = variance / n;',
            '  return Math.sqrt(variance);',
            '}']);
      list = Blockly.Yarn.valueToCode(block, 'LIST',
          Blockly.Yarn.ORDER_NONE) || '[]';
      code = functionName + '(' + list + ')';
      break;
    case 'RANDOM':
      var functionName = Blockly.Yarn.provideFunction_(
          'mathRandomList',
          ['function ' + Blockly.Yarn.FUNCTION_NAME_PLACEHOLDER_ +
              '(list) {',
            '  var x = Math.floor(Math.random() * list.length);',
            '  return list[x];',
            '}']);
      list = Blockly.Yarn.valueToCode(block, 'LIST',
          Blockly.Yarn.ORDER_NONE) || '[]';
      code = functionName + '(' + list + ')';
      break;
    default:
      throw Error('Unknown operator: ' + func);
  }
  return [code, Blockly.Yarn.ORDER_FUNCTION_CALL];
};

Blockly.Yarn['math_modulo'] = function(block) {
  // Remainder computation.
  var argument0 = Blockly.Yarn.valueToCode(block, 'DIVIDEND',
      Blockly.Yarn.ORDER_MODULUS) || '0';
  var argument1 = Blockly.Yarn.valueToCode(block, 'DIVISOR',
      Blockly.Yarn.ORDER_MODULUS) || '0';
  var code = argument0 + ' % ' + argument1;
  return [code, Blockly.Yarn.ORDER_MODULUS];
};

Blockly.Yarn['math_constrain'] = function(block) {
  // Constrain a number between two limits.
  var argument0 = Blockly.Yarn.valueToCode(block, 'VALUE',
      Blockly.Yarn.ORDER_COMMA) || '0';
  var argument1 = Blockly.Yarn.valueToCode(block, 'LOW',
      Blockly.Yarn.ORDER_COMMA) || '0';
  var argument2 = Blockly.Yarn.valueToCode(block, 'HIGH',
      Blockly.Yarn.ORDER_COMMA) || 'Infinity';
  var code = 'Math.min(Math.max(' + argument0 + ', ' + argument1 + '), ' +
      argument2 + ')';
  return [code, Blockly.Yarn.ORDER_FUNCTION_CALL];
};

Blockly.Yarn['math_random_int'] = function(block) {
  // Random integer between [X] and [Y].
  var argument0 = Blockly.Yarn.valueToCode(block, 'FROM',
      Blockly.Yarn.ORDER_COMMA) || '0';
  var argument1 = Blockly.Yarn.valueToCode(block, 'TO',
      Blockly.Yarn.ORDER_COMMA) || '0';
  var functionName = Blockly.Yarn.provideFunction_(
      'mathRandomInt',
      ['function ' + Blockly.Yarn.FUNCTION_NAME_PLACEHOLDER_ +
          '(a, b) {',
       '  if (a > b) {',
       '    // Swap a and b to ensure a is smaller.',
       '    var c = a;',
       '    a = b;',
       '    b = c;',
       '  }',
       '  return Math.floor(Math.random() * (b - a + 1) + a);',
       '}']);
  var code = functionName + '(' + argument0 + ', ' + argument1 + ')';
  return [code, Blockly.Yarn.ORDER_FUNCTION_CALL];
};

Blockly.Yarn['math_random_float'] = function(block) {
  // Random fraction between 0 and 1.
  return ['Math.random()', Blockly.Yarn.ORDER_FUNCTION_CALL];
};

Blockly.Yarn['math_atan2'] = function(block) {
  // Arctangent of point (X, Y) in degrees from -180 to 180.
  var argument0 = Blockly.Yarn.valueToCode(block, 'X',
      Blockly.Yarn.ORDER_COMMA) || '0';
  var argument1 = Blockly.Yarn.valueToCode(block, 'Y',
      Blockly.Yarn.ORDER_COMMA) || '0';
  return ['Math.atan2(' + argument1 + ', ' + argument0 + ') / Math.PI * 180',
      Blockly.Yarn.ORDER_DIVISION];
};
