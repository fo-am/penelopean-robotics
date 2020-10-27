Blockly.Yarn['math_number'] = function(block) {
  // Numeric value.
  var code = Number(block.getFieldValue('NUM'));
  var order = code >= 0 ? Blockly.Yarn.ORDER_ATOMIC :
              Blockly.Yarn.ORDER_UNARY_NEGATION;
  return [code, order];
};

Blockly.Yarn['math_arithmetic'] = function(block) {
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
