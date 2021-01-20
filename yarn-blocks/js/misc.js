function exportBlocks() {
    var xml = Blockly.Xml.workspaceToDom(workspace);
    var xml_text = Blockly.Xml.domToText(xml);      
    prompt("Copy-paste the XML code from here", xml_text);    
}
	
function importBlocks() {
  try {
    var xml_text = prompt("Please enter XML code", "");
    var xml = Blockly.Xml.textToDom(xml_text);
    workspace.clear();
    Blockly.Xml.domToWorkspace(xml, workspace);
  } catch (e) {
    alert(e);
  }
}

function exampleBlocks(id) {
  try {
      var xml = document.getElementById(id);
      workspace.clear();
      Blockly.Xml.domToWorkspace(xml, workspace);
  } catch (e) {
      alert(e);
  }
}

function selectedRobot() {
    var num=0;
    var robots=["a","b","c","d","e","f","g","h"]
    var ret=-1;
    robots.forEach(function (id) {
	var radio = document.getElementById('robot-'+id);
	if (radio.checked) ret=num;
	num++;
    });
    return ret;
}

var sending = false;

function transmitBlocks() {
    robot_id = selectedRobot();
    if (robot_id!=-1 && sending==false) {
	sending=true;
	document.getElementById("transmit").style.display="block";

	var code = "("+Blockly.Yarn.workspaceToCode(workspace)+")";
	
	var xhr = new XMLHttpRequest();
	xhr.open("POST", "/transmit-"+robot_id, true);
	xhr.setRequestHeader('Content-Type', 'application/json');
	xhr.addEventListener("load",function() {
	    console.log(xhr.status);
	    sending=false;
	    document.getElementById("transmit").style.display="none";
	    if (xhr.status!=200) {
	  	    document.getElementById("error").style.display="block";
		setTimeout(function() {
		    document.getElementById("error").style.display="none";
		}, 2000);
	    }
	});
	xhr.addEventListener("error",function() {
	    sending=false;
	    document.getElementById("transmit").style.display="none";
	    document.getElementById("error").style.display="block";
	    setTimeout(function() {
		document.getElementById("error").style.display="none";
	    }, 2000);
	});
	xhr.send(code);
    }
}


