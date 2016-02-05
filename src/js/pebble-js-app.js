function fetchMenu(longitude) {
  var response;
  var req = new XMLHttpRequest();
  req.open('GET', "http://www.kimonolabs.com/api/a1dy3kj0?apikey=", true);
  req.onload = function(e) {
    if (req.readyState == 4) {
      if(req.status == 200) {
        console.log(req.responseText);
        response = JSON.parse(req.responseText);
        var breakfast, lunch, dinner;
        //if (response && response.results && response.results.length > 0) {
          var cuisineResult = response.results;
          breakfast = cuisineResult.Breakfast[0].Breakfast.replace("\n", " ");
          lunch = cuisineResult.Lunch[0].Lunch.replace("\n", " ");
          dinner = cuisineResult.Dinner[0].Dinner.replace("\n", " ");
          console.log(breakfast);
          console.log(lunch);
          console.log(dinner);
          Pebble.sendAppMessage( { 0: breakfast, 1: lunch, 2: dinner});
        //}

      } else {
        console.log("Error");
      }
    }
  };
  req.send(null);
}

/*function locationSuccess(pos) {
  var coordinates = pos.coords;
  fetchWeather(coordinates.latitude, coordinates.longitude);
}*/

/*function locationError(err) {
  console.warn('location error (' + err.code + '): ' + err.message);
  Pebble.sendAppMessage({
    "city":"Loc Unavailable",
    "temperature":"N/A"
  });
}*/

//var locationOptions = { "timeout": 15000, "maximumAge": 60000 }; 

Pebble.addEventListener("ready",
                        function(e) {
                          console.log("connect!" + e.ready);
                          //locationWatcher = window.navigator.geolocation.watchPosition(locationSuccess, locationError, locationOptions);
                          console.log(e.type);
                          //Pebble.showSimpleNotificationOnPebble("BadApp", "I am running!");
                          fetchMenu(null);
                          //Pebble.sendAppMessage( { 0: "NOM NOMS", 1: "Special NOM NOMS", 2: "Just Dinner" });
                        });

/*var transactionId = Pebble.sendAppMessage( { "0": "NOM NOMS", "1": "Special NOM NOMS", "2": "Just Dinner" },
  function(e) {
    console.log("Successfully delivered message with transactionId=" + e.data.transactionId);
    Pebble.showSimpleNotificationOnPebble("Successful", "I am running!");
  },
  function(e) {
    console.log("Unable to deliver message with transactionId=" + e.data.transactionId +
      " Error is: " + e.error.message);
    Pebble.showSimpleNotificationOnPebble("Well fuck", "I am running!");
  }
);

*/
/*
Pebble.addEventListener("appmessage",
                        function(e) {
                          //window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
                          console.log(e.type);
                          //console.log(e.payload.temperature);
                          console.log("Om nom noms");
                        });

Pebble.addEventListener("webviewclosed",
                        function(e) {
                          console.log("webview closed");
                          console.log(e.type);
                          console.log(e.response);
                        });

*/
