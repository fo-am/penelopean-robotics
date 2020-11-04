Blockly.Yarn['logic_compare'] = function(block) {
  // Basic arithmetic operators, and power.
  var OPERATORS = {
    'EQ': ['eq? ', Blockly.Yarn.ORDER_ADDITION],
    'LT': ['< ', Blockly.Yarn.ORDER_SUBTRACTION],
    'LTE': ['<= ', Blockly.Yarn.ORDER_MULTIPLICATION],
    'GT': ['> ', Blockly.Yarn.ORDER_DIVISION],
    'GTE': ['>= ', Blockly.Yarn.ORDER_DIVISION],
    'SLT': ['s< ', Blockly.Yarn.ORDER_SUBTRACTION],
    'SLTE': ['s<= ', Blockly.Yarn.ORDER_MULTIPLICATION],
    'SGT': ['s> ', Blockly.Yarn.ORDER_DIVISION],
    'SGTE': ['s>= ', Blockly.Yarn.ORDER_DIVISION],
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

Blockly.Yarn['logic_when'] = function(block) {
  // If/elseif/else condition.
    var n = 0;
    var code = '', branchCode, conditionCode;
    conditionCode = Blockly.Yarn.valueToCode(block, 'BOOL',
					     Blockly.Yarn.ORDER_NONE) || '#f';
    branchCode = Blockly.Yarn.statementToCode(block, 'DO');
    if (Blockly.Yarn.STATEMENT_SUFFIX) {
	branchCode = Blockly.Yarn.prefixLines(
          Blockly.Yarn.injectId(Blockly.Yarn.STATEMENT_SUFFIX,
          block), Blockly.Yarn.INDENT) + branchCode;
    }
    code = '(when ' + conditionCode + "\n" + branchCode + ')';
    return code + '\n';
};

Blockly.Yarn['logic_if'] = function(block) {
    // If/elseif/else condition.
    var n = 0;
    var code = '', branchCode, conditionCode, elseCode;
    conditionCode = Blockly.Yarn.valueToCode(block, 'BOOL',
					     Blockly.Yarn.ORDER_NONE) || '#f';
    branchCode = Blockly.Yarn.statementToCode(block, 'DO');
    if (Blockly.Yarn.STATEMENT_SUFFIX) {
	branchCode = Blockly.Yarn.prefixLines(
            Blockly.Yarn.injectId(Blockly.Yarn.STATEMENT_SUFFIX,
				  block), Blockly.Yarn.INDENT) + branchCode;
    }
    
    elseCode = Blockly.Yarn.statementToCode(block, 'ELSE');
    if (Blockly.Yarn.STATEMENT_SUFFIX) {
	elseCode = Blockly.Yarn.prefixLines(
            Blockly.Yarn.injectId(Blockly.Yarn.STATEMENT_SUFFIX,
				  block), Blockly.Yarn.INDENT) + elseCode;
    }    
    code = '(cond \n (' + conditionCode + "\n" + branchCode + ")\n(else \n" + elseCode + '))';
    return code + '\n';
};

Blockly.Yarn['loops_forever'] = function(block) {
    var n = 0;
    var code = '', branchCode;
    branchCode = Blockly.Yarn.statementToCode(block, 'DO');
    if (Blockly.Yarn.STATEMENT_SUFFIX) {
	branchCode = Blockly.Yarn.prefixLines(
            Blockly.Yarn.injectId(Blockly.Yarn.STATEMENT_SUFFIX,
				  block), Blockly.Yarn.INDENT) + branchCode;
    }    
    code = '(forever \n'  + branchCode + ')';
    return code + '\n';
};

Blockly.Yarn['loops_while'] = function(block) {
    var n = 0;
    var code = '', branchCode, conditionCode;
    conditionCode = Blockly.Yarn.valueToCode(block, 'BOOL',
					     Blockly.Yarn.ORDER_NONE) || '#f';
    branchCode = Blockly.Yarn.statementToCode(block, 'DO');
    if (Blockly.Yarn.STATEMENT_SUFFIX) {
	branchCode = Blockly.Yarn.prefixLines(
          Blockly.Yarn.injectId(Blockly.Yarn.STATEMENT_SUFFIX,
          block), Blockly.Yarn.INDENT) + branchCode;
    }
    code = '(while ' + conditionCode + "\n" + branchCode + ')';
    return code + '\n';
};

Blockly.Yarn['loops_do'] = function(block) {
  var n = 0;
  var code = '', branchCode;
    branchCode = Blockly.Yarn.statementToCode(block, 'DO');
    if (Blockly.Yarn.STATEMENT_SUFFIX) {
	branchCode = Blockly.Yarn.prefixLines(
            Blockly.Yarn.injectId(Blockly.Yarn.STATEMENT_SUFFIX,
				  block), Blockly.Yarn.INDENT) + branchCode;
    }    
    code = '(do \n'  + branchCode + ')';
    return code + '\n';
};

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

Blockly.Yarn['actions_led'] = function(block) {
    // Basic arithmetic operators, and power.
    var OPERATORS = {
	'ON': ['1', Blockly.Yarn.ORDER_ADDITION],
	'OFF': ['0', Blockly.Yarn.ORDER_SUBTRACTION],
    };
    var tuple = OPERATORS[block.getFieldValue('VALUE')];
    var code;
    code = '(set! led ' + tuple[0] + ')';
    return code;
};

Blockly.Yarn['actions_wait'] = function(block) {
    var code;    
    var argument0 = Blockly.Yarn.valueToCode(block, 'A', 0) || '0';
    // current cycle rate is 50hz
    code = '(set! usr-d (* ' + argument0 + ' 50))\n(while (> usr-d 0) (set! usr-d (- usr-d 1)))'; 
    return code;
};

Blockly.Yarn['actions_walk'] = function(block) {
    // Basic arithmetic operators, and power.
    var OPERATORS = {
	"WALK-STOP":"walk-stop",
	"WALK-FORWARD":"walk-forward",
	"WALK-BACKWARD":"walk-backward",
	"TURN-LEFT":"turn-left",
	"TURN-RIGHT":"turn-right",
	"TURN-LEFT2":"turn-left2",
	"TURN-RIGHT2":"turn-right2",
	"WALK-SILLY":"walk-silly"			
    };
    var code;
    code = '(set! next-pattern ' + OPERATORS[block.getFieldValue('PATTERN')] + ')';
    return code;
};

Blockly.Yarn['actions_walk_steps'] = function(block) {
    // Basic arithmetic operators, and power.
    var OPERATORS = {
	"WALK-STOP":"walk-stop",
	"WALK-FORWARD":"walk-forward",
	"WALK-BACKWARD":"walk-backward",
	"TURN-LEFT":"turn-left",
	"TURN-RIGHT":"turn-right",
	"TURN-LEFT2":"turn-left2",
	"TURN-RIGHT2":"turn-right2",
	"WALK-SILLY":"walk-silly"			
    };
    var code;
    var argument0 = Blockly.Yarn.valueToCode(block, 'A', 0) || '0';

    code = '(set! next-pattern ' + OPERATORS[block.getFieldValue('PATTERN')] + ')\n' +
	'(set! step-count-reset 1)\n'+
	'(while (< step-count '+argument0+') 0)\n';
    return code;
};

Blockly.Yarn['actions_sleep'] = function(block) {
    return "(set! sleep 1)\n";
}

Blockly.Yarn['actions_walk_speed'] = function(block) {
    var code;
    var argument0 = Blockly.Yarn.valueToCode(block, 'A', 0) || '0';
    code = '(set! servo-ms-per-step ' + argument0 + ')\n';
    return code;
};

Blockly.Yarn['actions_servo_amp'] = function(block) {
    var code;
    var argument0 = Blockly.Yarn.valueToCode(block, 'A', 0) || '0';
    code = '(set! servo-' + block.getFieldValue('SERVO') + '-amp ' + argument0 + ')\n';
    return code;
};

Blockly.Yarn['actions_servo_bias'] = function(block) {
    var code;
    var argument0 = Blockly.Yarn.valueToCode(block, 'A', 0) || '0';
    code = '(set! servo-' + block.getFieldValue('SERVO') + '-bias ' + argument0 + ')\n';
    return code;
};

Blockly.Yarn['actions_servo_smooth'] = function(block) {
    var code;
    var argument0 = Blockly.Yarn.valueToCode(block, 'A', 0) || '0';
    code = '(set! servo-' + block.getFieldValue('SERVO') + '-smooth ' + argument0 + ')\n';
    return code;
};

Blockly.Yarn['sensor_accel'] = function(block) {
    var code;
    code = 'accel-' + block.getFieldValue('DIM');
    return [code, 0];
};

Blockly.Yarn['sensor_gyro'] = function(block) {
    var code;
    code = 'gyro-' + block.getFieldValue('DIM');
    return [code, 0];
};

Blockly.Yarn['sensor_magnetometer'] = function(block) {
    var code;
    code = 'comp-' + block.getFieldValue('DIM');
    return [code, 0];
};

Blockly.Yarn['sensor_compass'] = function(block) {
    return ['comp-angle', 0];
};

Blockly.Yarn['sensor_battery_level'] = function(block) {
    return ['vcc', 0];
};

Blockly.Yarn['sensor_temperature'] = function(block) {
    return ['temperature', 0];
};

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
