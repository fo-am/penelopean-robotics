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

function transmitBlocks() {
    var code = "("+Blockly.Yarn.workspaceToCode(workspace)+")";

      var xhr = new XMLHttpRequest();
    xhr.open("POST", "/transmit-1", true);
    xhr.setRequestHeader('Content-Type', 'application/json');
    xhr.send(code);
}

