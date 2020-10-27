/**
 * @license
 * Copyright 2012 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @fileoverview Generating Yarn for list blocks.
 * @author fraser@google.com (Neil Fraser)
 */
'use strict';

goog.provide('Blockly.Yarn.lists');

goog.require('Blockly.Yarn');


Blockly.Yarn['lists_create_empty'] = function(block) {
  // Create an empty list.
  return ['[]', Blockly.Yarn.ORDER_ATOMIC];
};

Blockly.Yarn['lists_create_with'] = function(block) {
  // Create a list with any number of elements of any type.
  var elements = new Array(block.itemCount_);
  for (var i = 0; i < block.itemCount_; i++) {
    elements[i] = Blockly.Yarn.valueToCode(block, 'ADD' + i,
        Blockly.Yarn.ORDER_COMMA) || 'null';
  }
  var code = '[' + elements.join(', ') + ']';
  return [code, Blockly.Yarn.ORDER_ATOMIC];
};

Blockly.Yarn['lists_repeat'] = function(block) {
  // Create a list with one element repeated.
  var functionName = Blockly.Yarn.provideFunction_(
      'listsRepeat',
      ['function ' + Blockly.Yarn.FUNCTION_NAME_PLACEHOLDER_ +
          '(value, n) {',
       '  var array = [];',
       '  for (var i = 0; i < n; i++) {',
       '    array[i] = value;',
       '  }',
       '  return array;',
       '}']);
  var element = Blockly.Yarn.valueToCode(block, 'ITEM',
      Blockly.Yarn.ORDER_COMMA) || 'null';
  var repeatCount = Blockly.Yarn.valueToCode(block, 'NUM',
      Blockly.Yarn.ORDER_COMMA) || '0';
  var code = functionName + '(' + element + ', ' + repeatCount + ')';
  return [code, Blockly.Yarn.ORDER_FUNCTION_CALL];
};

Blockly.Yarn['lists_length'] = function(block) {
  // String or array length.
  var list = Blockly.Yarn.valueToCode(block, 'VALUE',
      Blockly.Yarn.ORDER_MEMBER) || '[]';
  return [list + '.length', Blockly.Yarn.ORDER_MEMBER];
};

Blockly.Yarn['lists_isEmpty'] = function(block) {
  // Is the string null or array empty?
  var list = Blockly.Yarn.valueToCode(block, 'VALUE',
      Blockly.Yarn.ORDER_MEMBER) || '[]';
  return ['!' + list + '.length', Blockly.Yarn.ORDER_LOGICAL_NOT];
};

Blockly.Yarn['lists_indexOf'] = function(block) {
  // Find an item in the list.
  var operator = block.getFieldValue('END') == 'FIRST' ?
      'indexOf' : 'lastIndexOf';
  var item = Blockly.Yarn.valueToCode(block, 'FIND',
      Blockly.Yarn.ORDER_NONE) || '\'\'';
  var list = Blockly.Yarn.valueToCode(block, 'VALUE',
      Blockly.Yarn.ORDER_MEMBER) || '[]';
  var code = list + '.' + operator + '(' + item + ')';
  if (block.workspace.options.oneBasedIndex) {
    return [code + ' + 1', Blockly.Yarn.ORDER_ADDITION];
  }
  return [code, Blockly.Yarn.ORDER_FUNCTION_CALL];
};

Blockly.Yarn['lists_getIndex'] = function(block) {
  // Get element at index.
  // Note: Until January 2013 this block did not have MODE or WHERE inputs.
  var mode = block.getFieldValue('MODE') || 'GET';
  var where = block.getFieldValue('WHERE') || 'FROM_START';
  var listOrder = (where == 'RANDOM') ? Blockly.Yarn.ORDER_COMMA :
      Blockly.Yarn.ORDER_MEMBER;
  var list = Blockly.Yarn.valueToCode(block, 'VALUE', listOrder) || '[]';

  switch (where) {
    case ('FIRST'):
      if (mode == 'GET') {
        var code = list + '[0]';
        return [code, Blockly.Yarn.ORDER_MEMBER];
      } else if (mode == 'GET_REMOVE') {
        var code = list + '.shift()';
        return [code, Blockly.Yarn.ORDER_MEMBER];
      } else if (mode == 'REMOVE') {
        return list + '.shift();\n';
      }
      break;
    case ('LAST'):
      if (mode == 'GET') {
        var code = list + '.slice(-1)[0]';
        return [code, Blockly.Yarn.ORDER_MEMBER];
      } else if (mode == 'GET_REMOVE') {
        var code = list + '.pop()';
        return [code, Blockly.Yarn.ORDER_MEMBER];
      } else if (mode == 'REMOVE') {
        return list + '.pop();\n';
      }
      break;
    case ('FROM_START'):
      var at = Blockly.Yarn.getAdjusted(block, 'AT');
      if (mode == 'GET') {
        var code = list + '[' + at + ']';
        return [code, Blockly.Yarn.ORDER_MEMBER];
      } else if (mode == 'GET_REMOVE') {
        var code = list + '.splice(' + at + ', 1)[0]';
        return [code, Blockly.Yarn.ORDER_FUNCTION_CALL];
      } else if (mode == 'REMOVE') {
        return list + '.splice(' + at + ', 1);\n';
      }
      break;
    case ('FROM_END'):
      var at = Blockly.Yarn.getAdjusted(block, 'AT', 1, true);
      if (mode == 'GET') {
        var code = list + '.slice(' + at + ')[0]';
        return [code, Blockly.Yarn.ORDER_FUNCTION_CALL];
      } else if (mode == 'GET_REMOVE') {
        var code = list + '.splice(' + at + ', 1)[0]';
        return [code, Blockly.Yarn.ORDER_FUNCTION_CALL];
      } else if (mode == 'REMOVE') {
        return list + '.splice(' + at + ', 1);';
      }
      break;
    case ('RANDOM'):
      var functionName = Blockly.Yarn.provideFunction_(
          'listsGetRandomItem',
          ['function ' + Blockly.Yarn.FUNCTION_NAME_PLACEHOLDER_ +
              '(list, remove) {',
           '  var x = Math.floor(Math.random() * list.length);',
           '  if (remove) {',
           '    return list.splice(x, 1)[0];',
           '  } else {',
           '    return list[x];',
           '  }',
           '}']);
      code = functionName + '(' + list + ', ' + (mode != 'GET') + ')';
      if (mode == 'GET' || mode == 'GET_REMOVE') {
        return [code, Blockly.Yarn.ORDER_FUNCTION_CALL];
      } else if (mode == 'REMOVE') {
        return code + ';\n';
      }
      break;
  }
  throw Error('Unhandled combination (lists_getIndex).');
};

Blockly.Yarn['lists_setIndex'] = function(block) {
  // Set element at index.
  // Note: Until February 2013 this block did not have MODE or WHERE inputs.
  var list = Blockly.Yarn.valueToCode(block, 'LIST',
      Blockly.Yarn.ORDER_MEMBER) || '[]';
  var mode = block.getFieldValue('MODE') || 'GET';
  var where = block.getFieldValue('WHERE') || 'FROM_START';
  var value = Blockly.Yarn.valueToCode(block, 'TO',
      Blockly.Yarn.ORDER_ASSIGNMENT) || 'null';
  // Cache non-trivial values to variables to prevent repeated look-ups.
  // Closure, which accesses and modifies 'list'.
  function cacheList() {
    if (list.match(/^\w+$/)) {
      return '';
    }
    var listVar = Blockly.Yarn.variableDB_.getDistinctName(
        'tmpList', Blockly.VARIABLE_CATEGORY_NAME);
    var code = 'var ' + listVar + ' = ' + list + ';\n';
    list = listVar;
    return code;
  }
  switch (where) {
    case ('FIRST'):
      if (mode == 'SET') {
        return list + '[0] = ' + value + ';\n';
      } else if (mode == 'INSERT') {
        return list + '.unshift(' + value + ');\n';
      }
      break;
    case ('LAST'):
      if (mode == 'SET') {
        var code = cacheList();
        code += list + '[' + list + '.length - 1] = ' + value + ';\n';
        return code;
      } else if (mode == 'INSERT') {
        return list + '.push(' + value + ');\n';
      }
      break;
    case ('FROM_START'):
      var at = Blockly.Yarn.getAdjusted(block, 'AT');
      if (mode == 'SET') {
        return list + '[' + at + '] = ' + value + ';\n';
      } else if (mode == 'INSERT') {
        return list + '.splice(' + at + ', 0, ' + value + ');\n';
      }
      break;
    case ('FROM_END'):
      var at = Blockly.Yarn.getAdjusted(block, 'AT', 1, false,
          Blockly.Yarn.ORDER_SUBTRACTION);
      var code = cacheList();
      if (mode == 'SET') {
        code += list + '[' + list + '.length - ' + at + '] = ' + value + ';\n';
        return code;
      } else if (mode == 'INSERT') {
        code += list + '.splice(' + list + '.length - ' + at + ', 0, ' + value +
            ');\n';
        return code;
      }
      break;
    case ('RANDOM'):
      var code = cacheList();
      var xVar = Blockly.Yarn.variableDB_.getDistinctName(
          'tmpX', Blockly.VARIABLE_CATEGORY_NAME);
      code += 'var ' + xVar + ' = Math.floor(Math.random() * ' + list +
          '.length);\n';
      if (mode == 'SET') {
        code += list + '[' + xVar + '] = ' + value + ';\n';
        return code;
      } else if (mode == 'INSERT') {
        code += list + '.splice(' + xVar + ', 0, ' + value + ');\n';
        return code;
      }
      break;
  }
  throw Error('Unhandled combination (lists_setIndex).');
};

/**
 * Returns an expression calculating the index into a list.
 * @param {string} listName Name of the list, used to calculate length.
 * @param {string} where The method of indexing, selected by dropdown in Blockly
 * @param {string=} opt_at The optional offset when indexing from start/end.
 * @return {string|undefined} Index expression.
 * @private
 */
Blockly.Yarn.lists.getIndex_ = function(listName, where, opt_at) {
  if (where == 'FIRST') {
    return '0';
  } else if (where == 'FROM_END') {
    return listName + '.length - 1 - ' + opt_at;
  } else if (where == 'LAST') {
    return listName + '.length - 1';
  } else {
    return opt_at;
  }
};

Blockly.Yarn['lists_getSublist'] = function(block) {
  // Get sublist.
  var list = Blockly.Yarn.valueToCode(block, 'LIST',
      Blockly.Yarn.ORDER_MEMBER) || '[]';
  var where1 = block.getFieldValue('WHERE1');
  var where2 = block.getFieldValue('WHERE2');
  if (where1 == 'FIRST' && where2 == 'LAST') {
    var code = list + '.slice(0)';
  } else if (list.match(/^\w+$/) ||
      (where1 != 'FROM_END' && where2 == 'FROM_START')) {
    // If the list is a variable or doesn't require a call for length, don't
    // generate a helper function.
    switch (where1) {
      case 'FROM_START':
        var at1 = Blockly.Yarn.getAdjusted(block, 'AT1');
        break;
      case 'FROM_END':
        var at1 = Blockly.Yarn.getAdjusted(block, 'AT1', 1, false,
            Blockly.Yarn.ORDER_SUBTRACTION);
        at1 = list + '.length - ' + at1;
        break;
      case 'FIRST':
        var at1 = '0';
        break;
      default:
        throw Error('Unhandled option (lists_getSublist).');
    }
    switch (where2) {
      case 'FROM_START':
        var at2 = Blockly.Yarn.getAdjusted(block, 'AT2', 1);
        break;
      case 'FROM_END':
        var at2 = Blockly.Yarn.getAdjusted(block, 'AT2', 0, false,
            Blockly.Yarn.ORDER_SUBTRACTION);
        at2 = list + '.length - ' + at2;
        break;
      case 'LAST':
        var at2 = list + '.length';
        break;
      default:
        throw Error('Unhandled option (lists_getSublist).');
    }
    code = list + '.slice(' + at1 + ', ' + at2 + ')';
  } else {
    var at1 = Blockly.Yarn.getAdjusted(block, 'AT1');
    var at2 = Blockly.Yarn.getAdjusted(block, 'AT2');
    var getIndex_ = Blockly.Yarn.lists.getIndex_;
    var wherePascalCase = {'FIRST': 'First', 'LAST': 'Last',
        'FROM_START': 'FromStart', 'FROM_END': 'FromEnd'};
    var functionName = Blockly.Yarn.provideFunction_(
        'subsequence' + wherePascalCase[where1] + wherePascalCase[where2],
        ['function ' + Blockly.Yarn.FUNCTION_NAME_PLACEHOLDER_ +
            '(sequence' +
            // The value for 'FROM_END' and'FROM_START' depends on `at` so
            // we add it as a parameter.
            ((where1 == 'FROM_END' || where1 == 'FROM_START') ? ', at1' : '') +
            ((where2 == 'FROM_END' || where2 == 'FROM_START') ? ', at2' : '') +
            ') {',
          '  var start = ' + getIndex_('sequence', where1, 'at1') + ';',
          '  var end = ' + getIndex_('sequence', where2, 'at2') + ' + 1;',
          '  return sequence.slice(start, end);',
          '}']);
    var code = functionName + '(' + list +
        // The value for 'FROM_END' and 'FROM_START' depends on `at` so we
        // pass it.
        ((where1 == 'FROM_END' || where1 == 'FROM_START') ? ', ' + at1 : '') +
        ((where2 == 'FROM_END' || where2 == 'FROM_START') ? ', ' + at2 : '') +
        ')';
  }
  return [code, Blockly.Yarn.ORDER_FUNCTION_CALL];
};

Blockly.Yarn['lists_sort'] = function(block) {
  // Block for sorting a list.
  var list = Blockly.Yarn.valueToCode(block, 'LIST',
      Blockly.Yarn.ORDER_FUNCTION_CALL) || '[]';
  var direction = block.getFieldValue('DIRECTION') === '1' ? 1 : -1;
  var type = block.getFieldValue('TYPE');
  var getCompareFunctionName = Blockly.Yarn.provideFunction_(
      'listsGetSortCompare',
      ['function ' + Blockly.Yarn.FUNCTION_NAME_PLACEHOLDER_ +
          '(type, direction) {',
       '  var compareFuncs = {',
       '    "NUMERIC": function(a, b) {',
       '        return Number(a) - Number(b); },',
       '    "TEXT": function(a, b) {',
       '        return a.toString() > b.toString() ? 1 : -1; },',
       '    "IGNORE_CASE": function(a, b) {',
       '        return a.toString().toLowerCase() > ' +
          'b.toString().toLowerCase() ? 1 : -1; },',
       '  };',
       '  var compare = compareFuncs[type];',
       '  return function(a, b) { return compare(a, b) * direction; }',
       '}']);
  return [list + '.slice().sort(' +
      getCompareFunctionName + '("' + type + '", ' + direction + '))',
      Blockly.Yarn.ORDER_FUNCTION_CALL];
};

Blockly.Yarn['lists_split'] = function(block) {
  // Block for splitting text into a list, or joining a list into text.
  var input = Blockly.Yarn.valueToCode(block, 'INPUT',
      Blockly.Yarn.ORDER_MEMBER);
  var delimiter = Blockly.Yarn.valueToCode(block, 'DELIM',
      Blockly.Yarn.ORDER_NONE) || '\'\'';
  var mode = block.getFieldValue('MODE');
  if (mode == 'SPLIT') {
    if (!input) {
      input = '\'\'';
    }
    var functionName = 'split';
  } else if (mode == 'JOIN') {
    if (!input) {
      input = '[]';
    }
    var functionName = 'join';
  } else {
    throw Error('Unknown mode: ' + mode);
  }
  var code = input + '.' + functionName + '(' + delimiter + ')';
  return [code, Blockly.Yarn.ORDER_FUNCTION_CALL];
};

Blockly.Yarn['lists_reverse'] = function(block) {
  // Block for reversing a list.
  var list = Blockly.Yarn.valueToCode(block, 'LIST',
      Blockly.Yarn.ORDER_FUNCTION_CALL) || '[]';
  var code = list + '.slice().reverse()';
  return [code, Blockly.Yarn.ORDER_FUNCTION_CALL];
};
