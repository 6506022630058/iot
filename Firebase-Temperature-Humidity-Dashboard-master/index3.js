// Your web app's Firebase configuration
var firebaseConfig = {
        apiKey: "AIzaSyCflwLcZDItrDyvrpAX1jRJOvlV6yN7HEQ",
        authDomain: "fir-dht-2024-ce7d7.firebaseapp.com",
        projectId: "fir-dht-2024-ce7d7",
        storageBucket: "fir-dht-2024-ce7d7.appspot.com",
        messagingSenderId: "664651908781",
        appId: "1:664651908781:web:f7db6274f5e661c31ea824",
        databaseURL : "https://fir-dht-2024-ce7d7-default-rtdb.asia-southeast1.firebasedatabase.app"
      };
      // Initialize Firebase
      firebase.initializeApp(firebaseConfig);
      // Set database variable
      var database = firebase.database()
      function save() {
        var humidity = document.getElementById('humidity').value
        var led = document.getElementById('led').value
        var light = document.getElementById('light').value
        var temperature = document.getElementById('temperature').value
        var time = document.getElementById('time').value
        var times = document.getElementById('times').value
      
        database.ref('logs/' + id).set({
          humidity : humidity,
          led : led,
          light : light,
          temperature : temperature,
          time : time,
          times : times
        })
        alert('Saved')
      }
      function get() {
        // var username = document.getElementById('username').value
        var logs = database.ref('logs/log_1727009638')
        logs.on('value', function(snapshot) {
          var data = snapshot.val()
          console.log(logs)
        })
      }