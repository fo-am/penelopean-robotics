var LOGIC_COL = 100;
var MATHS_COL = 150;
var ACTIONS_COL = 200;
var SENSORS_COL = 250;
var VARIABLES_COL = 300;

Blockly.Blocks['logic_compare'] = {
    init: function() {
	this.jsonInit({
	    "type": "logic_compare",
	    "message0": "%1 %2 %3",
	    "args0": [
		{
		    "type": "input_value",
		    "name": "A"
		},
		{
		    "type": "field_dropdown",
		    "name": "OP",
		    "options": [["=", "EQ"],
				["\u2260", "NEQ"],
				["\u200F<", "LT"],
				["\u200F\u2264", "LTE"],
				["\u200F>", "GT"],
				["\u200F\u2265", "GTE"],
				["S\u200F<", "SLT"],
				["S\u200F\u2264", "SLTE"],
				["S\u200F>", "SGT"],
				["S\u200F\u2265", "SGTE"]
		    ]
		},
		{
		    "type": "input_value",
		    "name": "B"
		}
	    ],
	    "inputsInline": true,
	    "output": "Boolean",
	    "colour": MATHS_COL,
	    "helpUrl": "%{BKY_LOGIC_COMPARE_HELPURL}",
	    "extensions": ["logic_compare", "logic_op_tooltip"]
	})}}

Blockly.Blocks['logic_when'] = {
    init: function() {
	this.jsonInit({
	    "type": "logic_when",
	    "message0": "when %1 %2",
	    "args0": [
		{
		    "type": "input_value",
		    "name": "BOOL",
		    "check": "Boolean"
		},
		{
		    "type": "input_statement",
		    "name": "DO"
		}],
	    "colour": LOGIC_COL,
	    "previousStatement": null,
	    "nextStatement": null,
	})}}


Blockly.Blocks['logic_if'] = {
    init: function() {
	this.jsonInit({
	    "type": "logic_if",
	    "message0": "if %1 %2 else %3",
	    "args0": [
		{
		    "type": "input_value",
		    "name": "BOOL",
		    "check": "Boolean"
		},
		{
		    "type": "input_statement",
		    "name": "DO"
		},
		{
		    "type": "input_statement",
		    "name": "ELSE"
		}],
	    "colour": LOGIC_COL,
	    "previousStatement": null,
	    "nextStatement": null,
	})}}

Blockly.Blocks['loops_forever'] = {
    init: function() {
	this.jsonInit({
	    "type": "loops_forever",
	    "message0": "forever %1",
	    "args0": [
		{
		    "type": "input_statement",
		    "name": "DO"
		}],
	    "colour": LOGIC_COL,
	})}}


Blockly.Blocks['loops_while'] = {
    init: function() {
	this.jsonInit({
	    "type": "loops_while",
	    "message0": "while %1 %2",
	    "args0": [
		{
		    "type": "input_value",
		    "name": "BOOL",
		    "check": "Boolean"
		},
		{
		    "type": "input_statement",
		    "name": "DO"
		}],
	    "colour": LOGIC_COL,
	    "previousStatement": null,
	    "nextStatement": null,
	})}}

Blockly.Blocks['loops_do'] = {
    init: function() {
	this.jsonInit({
	    "type": "loops_do",
	    "message0": "do %1",
	    "args0": [
		{
		    "type": "input_statement",
		    "name": "DO"
		}],
	    "colour": LOGIC_COL,
	    "previousStatement": null,
	    "nextStatement": null,
	})}}

Blockly.Blocks['math_number'] = {
    init: function() {
	this.jsonInit({
	    "type": "math_number",
	    "message0": "%1",
	    "args0": [{
		"type": "field_number",
		"name": "NUM",
		"value": 0
	    }],
	    "output": "Number",
	    "helpUrl": "%{BKY_MATH_NUMBER_HELPURL}",
	    "colour": MATHS_COL,
	    "tooltip": "%{BKY_MATH_NUMBER_TOOLTIP}",
	    "extensions": ["parent_tooltip_when_inline"]
	})}}

Blockly.Blocks['math_arithmetic'] = {
    init: function() {
	this.jsonInit({
	    "type": "math_arithmetic",
	    "message0": "%1 %2 %3",
	    "args0": [
		{
		    "type": "input_value",
		    "name": "A",
		    "check": "Number"
		},
		{
		    "type": "field_dropdown",
		    "name": "OP",
		    "options": [
			["%{BKY_MATH_ADDITION_SYMBOL}", "ADD"],
			["%{BKY_MATH_SUBTRACTION_SYMBOL}", "MINUS"],
			["%{BKY_MATH_MULTIPLICATION_SYMBOL}", "MULTIPLY"],
			["%{BKY_MATH_DIVISION_SYMBOL}", "DIVIDE"],
			["modulo", "MOD"]
		    ]
		},
		{
		    "type": "input_value",
		    "name": "B",
		    "check": "Number"
		}
	    ],
	    "inputsInline": true,
	    "output": "Number",
	    "colour": MATHS_COL,
	    "helpUrl": "%{BKY_MATH_ARITHMETIC_HELPURL}",
	    "extensions": ["math_op_tooltip"]
	})}};

Blockly.Blocks['math_bitwise'] = {
    init: function() {
	this.jsonInit({
	    "type": "math_bitwise",
	    "message0": "%1 %2 %3",
	    "args0": [
		{
		    "type": "input_value",
		    "name": "A",
		    "check": "Number"
		},
		{
		    "type": "field_dropdown",
		    "name": "OP",
		    "options": [
			["and", "AND"],
			["or", "OR"],
			["xor", "XOR"],
			["bitwise-not", "BITWISE-NOT"],
			["not", "NOT"],
			["<<", "SHIFT-LEFT"],
			[">>", "SHIFT-RIGHT"],
			
		    ]
		},
		{
		    "type": "input_value",
		    "name": "B",
		    "check": "Number"
		}
	    ],
	    "inputsInline": true,
	    "output": "Number",
	    "colour": MATHS_COL,
	})}};

Blockly.Blocks['actions_led'] = {
    init: function() {
	this.jsonInit({
	    "type": "actions_led",
	    "message0": "led %1",
	    "args0": [
		{
		    "type": "field_dropdown",
		    "name": "VALUE",
		    "options": [["on","ON"],
				["off","OFF"]]
		}],
	    "colour": ACTIONS_COL,
	    "previousStatement": null,
	    "nextStatement": null,
	})}};


Blockly.Blocks['actions_walk'] = {
    init: function() {
	this.jsonInit({
	    "type": "actions_walk",
	    "message0": "walk %1",
	    "args0": [
		{
		    "type": "field_dropdown",
		    "name": "PATTERN",
		    "options": [["stop","WALK-STOP"],
				["forward","WALK-FORWARD"],
				["backward","WALK-BACKWARD"],
				["turn-left","TURN-LEFT"],
				["turn-right","TURN-RIGHT"],
				["turn-left2","TURN-LEFT2"],
				["turn-right2","TURN-RIGHT2"],
				["silly","WALK-SILLY"]]			
		}
	    ],
	    "colour": ACTIONS_COL,
	    "previousStatement": null,
	    "nextStatement": null,
	})}};

Blockly.Blocks['actions_walk_steps'] = {
    init: function() {
	this.jsonInit({
	    "type": "actions_walk_steps",
	    "message0": "walk %1 for %2 steps",
	    "args0": [
		{
		    "type": "field_dropdown",
		    "name": "PATTERN",
		    "options": [["stop","WALK-STOP"],
				["forward","WALK-FORWARD"],
				["backward","WALK-BACKWARD"],
				["turn-left","TURN-LEFT"],
				["turn-right","TURN-RIGHT"],
				["turn-left2","TURN-LEFT2"],
				["turn-right2","TURN-RIGHT2"],
				["silly","WALK-SILLY"]]			
		},
		{
		    "type": "input_value",
		    "name": "A",
		    "check": "Number"
		},],
	    "colour": ACTIONS_COL,
	    "previousStatement": null,
	    "nextStatement": null,
	})}};


Blockly.Blocks['actions_sleep'] = {
    init: function() {
	this.jsonInit({
	    "type": "actions_sleep",
	    "message0": "sleep",
	    "args0": [],
	    "colour": ACTIONS_COL,
	    "previousStatement": null,
	    "nextStatement": null,
	})}};

Blockly.Blocks['actions_walk_speed'] = {
    init: function() {
	this.jsonInit({
	    "type": "actions_led",
	    "message0": "walk speed %1",
	    "args0": [
		{
		    "type": "input_value",
		    "name": "A",
		    "check": "Number"
		}],
	    "colour": ACTIONS_COL,
	    "previousStatement": null,
	    "nextStatement": null,
	})}};

Blockly.Blocks['actions_servo_amp'] = {
    init: function() {
	this.jsonInit({
	    "type": "actions_servo_amp",
	    "message0": "servo %1 amp %2",
	    "args0": [
		{
		    "type": "field_dropdown",
		    "name": "SERVO",
		    "options": [["front","1"],
				["middle","2"],
				["back","3"]]
		},
		{
		    "type": "input_value",
		    "name": "A",
		    "check": "Number"
		}],
	    "colour": ACTIONS_COL,
	    "previousStatement": null,
	    "nextStatement": null,
	})}};

Blockly.Blocks['actions_servo_bias'] = {
    init: function() {
	this.jsonInit({
	    "type": "actions_servo_bias",
	    "message0": "servo %1 bias %2",
	    "args0": [
		{
		    "type": "field_dropdown",
		    "name": "SERVO",
		    "options": [["front","1"],
				["middle","2"],
				["back","3"]]
		},
		{
		    "type": "input_value",
		    "name": "A",
		    "check": "Number"
		}],
	    "colour": ACTIONS_COL,
	    "previousStatement": null,
	    "nextStatement": null,
	})}};

Blockly.Blocks['actions_servo_smooth'] = {
    init: function() {
	this.jsonInit({
	    "type": "actions_servo_smooth",
	    "message0": "servo %1 smooth %2",
	    "args0": [
		{
		    "type": "field_dropdown",
		    "name": "SERVO",
		    "options": [["front","1"],
				["middle","2"],
				["back","3"]]
		},
		{
		    "type": "input_value",
		    "name": "A",
		    "check": "Number"
		}],
	    "colour": ACTIONS_COL,
	    "previousStatement": null,
	    "nextStatement": null,
	})}};


Blockly.Blocks['sensor_accel'] = {
    init: function() {
	this.jsonInit({
	    "type": "sensor_accel",
	    "message0": "accelerometer %1",
	    "args0": [
		{
		    "type": "field_dropdown",
		    "name": "DIM",
		    "options": [["x","x"],
				["y","y"],
				["z","z"]]
		}
	    ],
	    "output": "Number",
	    "colour": SENSORS_COL,
	})}};

Blockly.Blocks['sensor_gyro'] = {
    init: function() {
	this.jsonInit({
	    "type": "sensor_gyro",
	    "message0": "gyroscope %1",
	    "args0": [
		{
		    "type": "field_dropdown",
		    "name": "DIM",
		    "options": [["x","x"],
				["y","y"],
				["z","z"]]
		}
	    ],
	    "output": "Number",
	    "colour": SENSORS_COL,
	})}};

Blockly.Blocks['sensor_magnetometer'] = {
    init: function() {
	this.jsonInit({
	    "type": "sensor_magnetometer",
	    "message0": "magnetometer %1",
	    "args0": [
		{
		    "type": "field_dropdown",
		    "name": "DIM",
		    "options": [["x","x"],
				["y","y"],
				["z","z"]]
		}
	    ],
	    "output": "Number",
	    "colour": SENSORS_COL,
	})}};

Blockly.Blocks['sensor_compass'] = {
    init: function() {
	this.jsonInit({
	    "type": "sensor_compass",
	    "message0": "compass heading",
	    "args0": [],
	    "output": "Number",
	    "colour": SENSORS_COL,
	})}};

Blockly.Blocks['sensor_battery_level'] = {
    init: function() {
	this.jsonInit({
	    "type": "sensor_battery_level",
	    "message0": "battery voltage",
	    "args0": [],
	    "output": "Number",
	    "colour": SENSORS_COL,
	})}};

Blockly.Blocks['sensor_temperature'] = {
    init: function() {
	this.jsonInit({
	    "type": "sensor_temperature",
	    "message0": "temperature",
	    "args0": [],
	    "output": "Number",
	    "colour": SENSORS_COL,
	})}};

Blockly.Blocks['variables_get'] = {
    init: function() {
	this.jsonInit({
	    "type": "variables_get",
	    "message0": "%1",
	    "args0": [
		{
		    "type": "field_variable",
		    "name": "VAR",
		    "variable": "%{BKY_VARIABLES_DEFAULT_NAME}"
		}
	    ],
	    "output": null,
	    "colour": VARIABLES_COL,
	    "helpUrl": "%{BKY_VARIABLES_GET_HELPURL}",
	    "tooltip": "%{BKY_VARIABLES_GET_TOOLTIP}",
	    "extensions": ["contextMenu_variableSetterGetter"]
	})}};

Blockly.Blocks['variables_set'] = {
    init: function() {
	this.jsonInit({
	    "type": "variables_set",
	    "message0": "%{BKY_VARIABLES_SET}",
	    "args0": [
		{
		    "type": "field_variable",
		    "name": "VAR",
		    "variable": "%{BKY_VARIABLES_DEFAULT_NAME}"
		},
		{
		    "type": "input_value",
		    "name": "VALUE"
		}
	    ],
	    "previousStatement": null,
	    "nextStatement": null,
	    "colour": VARIABLES_COL,
	    "tooltip": "%{BKY_VARIABLES_SET_TOOLTIP}",
	    "helpUrl": "%{BKY_VARIABLES_SET_HELPURL}",
	    "extensions": ["contextMenu_variableSetterGetter"]
	})}};



/*


-------------------------------
variables

set!

-------------------------------
procedures

defun
defconst


*/
