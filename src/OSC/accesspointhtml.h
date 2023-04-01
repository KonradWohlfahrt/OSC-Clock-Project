/*
  OSC-PROJECT-FILES
  Created by Donut Studio and Pr0metheuz_, April 01, 2023.
  Released into the public domain.
*/

/*
  --- --- --- --- --- --- HTML --- --- --- --- --- ---
*/
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1" charset="UTF-8">
    <style>
      html {
       font-family: Arial;
       display: inline-block;
       margin: 0px auto;
       text-align: center;
      }
      h1 { font-size: 2rem; }
      pre { font-size: 1rem }
      .units { font-size: 1.2rem; }
      .labels{
        font-size: 1.5rem;
        vertical-align:middle;
        padding-bottom: 15px;
      }
    </style>
  </head>

	<body>
    <h1>Overengineered Simple Clock</h1>
    <h2>WIFI-Data</h2>
        
    <form>
      <label for="SSID">WiFi-name (SSID):</label>
      <input type="text" id='ssid' name="SSID" value="" maxlength="32">
      
      <br/><br/>
      
      <label for="PASSWORD">WiFi-password:</label>
      <input type="text" id='pass' name="PASSWORD" value="" maxlength="32">
      
      <br/><br/>
      
      <label for="APIKEY">Weather API Key:</label>
      <input type="text" id='api' name="APIKEY" value="" maxlength="64">
      
      <br/><br/>
      
      <button>Submit</button>
    </form>

    <hr style="margin:10px">
    
    <form>
      <button name="SCAN">Search for networks</button>
    </form>
    <pre id='text'></pre>

    <script>
      getData('ssid', 'ssid');
      getData('pass', 'password');
      getData('api', 'apikey');
      getData('text', 'scan');

      function getData(elementID, url) 
      {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            if (elementID == 'text')
              document.getElementById(elementID).innerHTML = this.responseText;
            else
              document.getElementById(elementID).value = this.responseText;
          }
        };
        xhttp.open('GET', url, true);
        xhttp.send();
      }
    </script>

	</body>
</html>)rawliteral";