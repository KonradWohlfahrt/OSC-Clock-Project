/*
  OSC-PROJECT-FILES
  Created by Donut Studio and Pr0metheuz_, January 18, 2023.
  Released into the public domain.
*/

/*
  --- --- --- --- --- --- HTML --- --- --- --- --- ---
*/
// urls: temp, humi, stat, spee, brig, text, cust
// ids: temperature, humidity, state, speed, brightness, text, custom
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1" charset="UTF-8">
    <title>Matrix Display</title>
    <style>
      html {
       font-family: Arial;
       display: inline-block;
       margin: 0px auto;
       text-align: center;
      }
      h1 { font-size: 2rem; }
      .textInp { width: 15rem; }
      .units { font-size: 1.2rem; }
      .labels{
        font-size: 1.5rem;
        vertical-align:middle;
        padding-bottom: 15px;
      }
      .colform { float:left; width:50%;}
    </style>
  </head>

	<body>
    <h1>Matrix Display</h1>
    <h2>measurements</h2>
    <p id='temperature'>Temperature: -</p>
    <p id='humidity'>Humidity: -</p>
        
    <hr style="margin:10px">
        
    <h2>text selection</h2>
    <form>
      <p id='text'>Current text: </p>
      <label for="SELECTION">display text:</label>
      <select name="SELECTION">
        <option value="1">text 1</option>
        <option value="2">text 2</option>
        <option value="3">text 3</option>
        <option value="4">text 4</option>
        <option value="5">text 5</option>
        <option value="6">input text</option>
      </select>
      <button>Submit</button>
    </form>
    
    <h2>text input</h2>
    <form>
      <input class="textInp" id='custom' type="text" name="CUSTOMTEXT" value="" placeholder="input">
      <button>Submit</button>
    </form>
    
    <br/><br/>
    <hr style="margin:10px">
        
        
    <h2>display status</h2>
    <form>
      <p id='state'>display: on</p>
      <button name="TOGGLE">toggle display</button>
    </form>


    <form class="colform">
      <p>fast --- slow</p>
      <input id='speed' type="range" name="SPEED" min="10" max="100" value="50">
      <br/><br/>
      <button>Submit</button>
    </form>

    <form class="colform">
      <p>dark --- bright</p>
      <input id='brightness' type="range" name="BRIGHTNESS" min="0" max="15" value="4">
      <br/><br/>
      <button>Submit</button>
    </form>

		<br/>-<br/>

    <script>
      getData('state', 'stat');
      getData('speed', 'spee');
      getData('brightness', 'brig');
      getData('text', 'text');
      getData('custom', 'cust');

      setInterval(function() 
      {
        getData('temperature', 'temp');
        getData('humidity', 'humi');
      }, 2000);
      function getData(elementID, url) {
        var req = new XMLHttpRequest();
        req.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            if (url != 'spee' && url != 'brig' && url != 'cust')
              document.getElementById(elementID).innerHTML = this.responseText;
            else
              document.getElementById(elementID).value = this.responseText;
          }
        };
        req.open('GET', url, true);
        req.send();
      }
    </script>
	</body>
</html>)rawliteral";