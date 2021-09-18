// This file is required by the index.html file and will
// be executed in the renderer process for that window.
// All of the Node.js APIs are available in this process.

const serialport = require('serialport');
const Readline = require('@serialport/parser-readline');
const tableify = require('tableify');
const dateFormat = require("dateformat");

var port;

async function listSerialPorts() {
  await serialport.list().then((ports, err) => {
    if(err) {
      document.getElementById('error').textContent = err.message;
      return
    } else {
      document.getElementById('error').textContent = '';
    }
    console.log('ports', ports);

    if (ports.length === 0) {
      document.getElementById('error').textContent = 'No ports discovered';
    }
    else {
      var portPath = ports[0].path;
      console.log("connect to " + portPath);

      port = new serialport(portPath, function (err) {
        if (err) {
          return console.log('Error: ', err.message);
        }
      })

      const parser = new Readline();
      port.pipe(parser);
      parser.on('data', serialLineRead);

      port.on('error', function(err) {
        console.log('Error: ', err.message);
      })
    }

    tableHTML = tableify(ports);
    document.getElementById('ports').innerHTML = tableHTML;

    var now = new Date();
    var command  = dateFormat(now,"d-m-yyyy HH:M:s", true) + " 0 0"; // select the first account by default
    serialLineWrite(command);
  })
}

function serialLineRead(line){
  console.log(line);
}

function serialLineWrite(line){
  port.write(line, function(err) {
    if (err) {
      return console.log('Error on write: ', err.message);
    }
    console.log('sent: ' + line);
  })
}


function setAccount(accountIndex){
  var now = new Date();
  var doTypeValue = (document.getElementById("doType").checked) ? "1" : "0";
  var command  = dateFormat(now,"d-m-yyyy HH:M:s", true) + " " + accountIndex + " " + doTypeValue;
  serialLineWrite(command);
  document.getElementById(accountIndex).blur();
}

listSerialPorts();
