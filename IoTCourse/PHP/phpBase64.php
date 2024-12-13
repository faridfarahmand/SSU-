<?php

// Change this accordingly
$servername = "localhost";
$username = "id";
$password = "EE#";
$database = "idn";

$conn = new mysqli($servername, $username, $password, $database);

if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Query Sensor_Data table
$data_query = "SELECT * FROM Sensor_Data";
$data_result = $conn->query($data_query);

// Query Sensor_Register table
$register_query = "SELECT * FROM Sensor_register";
$register_result = $conn->query($register_query);


if (isset($_GET['base'])) {
    $encoded_data = $_GET['base'];  // the parameters start with base
    $decoded_data = base64_decode($encoded_data);
    echo "passed encode/decode stage \n";
    
    $keyValuePairs = explode('&', $decoded_data);
    
    // Initialize an empty array to store the extracted variables
    $extractedVariables = array();
    
    foreach ($keyValuePairs as $pair) {
        // Split each pair into key and value using '=' as a separator
        list($key, $value) = explode('=', $pair);
        // Assign the key-value pair to the extractedVariables array
        $extractedVariables[$key] = urldecode($value); // URL decode the value
    }
    
    $node_name = $extractedVariables['node_name'];
    $time_received = $extractedVariables['time_received'];
    $temperature = $extractedVariables['temperature'];
    $humidity = $extractedVariables['humidity'];

    // Check if the data is not empty
    if (!empty($node_name) && !empty($time_received) && !empty($temperature) && !empty($humidity)) {
        // Create the SQL query
        $sql = "INSERT INTO Sensor_Data (node_name, time_received, temperature, humidity)
                VALUES ('$node_name', '$time_received', $temperature, $humidity)";

        // Execute the SQL query
        if ($conn->query($sql) === TRUE) {
            echo "New record created successfully";
        } 
        else {
            echo "Error: " . $sql . "<br>" . $conn->error;
        }
    } 
    else {
        echo "Data fields are empty. Please provide valid data.";
    }
}
else {
    echo "skipped encode/decode stage \n";
}


?>

<!DOCTYPE html>
<html>
<head>
    <title>Database Data</title>
    <style>
        /* CSS styles for table formatting */
        body {
            background-color: #c7c6c5;
            font-family: Arial, sans-serif;
        }
        table {
            border-collapse: collapse;
            width: 40%;
            margin: 20px;
            font-family: Arial, sans-serif;

        }

        th, td {
            border: 2px solid #000000;
            text-align: left;
            padding: 10px;
        }

        th {
            background-color: #a1a1a1;
            font-weight: bold; 
            text-align: left;
        }

        tr:nth-child(even) {
            background-color: #beced4; 
        }

        tr:nth-child(odd) {
            background-color: #b6c8cf; 
        }
        h1 {
            text-align: left; 
        }
    </style>
</head>
<body>
    <h1>Sensor Data</h1>
        
    <table>
        <tr>
            <th>Node Name</th>
            <th>Time Received</th>
            <th>Temperature</th>
            <th>Humidity</th>
        </tr>
        <?php
        // Retrieve data for a specific Node Name
        $specific_data_query = "SELECT * FROM Sensor_Data WHERE node_name = 'node-1'";
        $specific_data_result = $conn->query($specific_data_query);

        $temperature_total = 0;
        $temperature_count = 0;
        $humidity_total = 0;
        $humidity_count = 0;
            
        while ($row = $data_result->fetch_assoc()) {
            echo "<tr>";
            echo "<td>" . $row["node_name"] . "</td>";
            echo "<td>" . $row["time_received"] . "</td>";
            echo "<td>" . $row["temperature"] . "</td>";
            echo "<td>" . $row["humidity"] . "</td>";
            
            echo "</tr>";
            $temperature_total += $row["temperature"];
            $temperature_count++;
            $humidity_total += $row["humidity"];
            $humidity_count++;
        }
        
        // Calculate and display the average temperature
        if ($temperature_count > 0) {
            $average_temperature = $temperature_total / $temperature_count;
            $average_humidity = $humidity_total / $humidity_count;
        }
        ?>
    </table>
    <p><b>The average temperature for node 1:</b>
        <?php 
        echo "<td>" . number_format($average_temperature, 0) . "</td>"; 
        ?>
        F
    </p>
    <p><b>The average humidity for node 1:</b>
        <?php 
        echo "<td>" . number_format($average_humidity, 0) . "</td>"; 
        ?>
        %
    </p>
    

    <h1>Sensor Register</h1>
    <table>
        <tr>
            <th>Node Name</th>
            <th>Manufacturer</th>
            <th>Longitude</th>
            <th>Latitude</th>
        </tr>
        <?php
        while ($row = $register_result->fetch_assoc()) {
            echo "<tr>";
            echo "<td>" . $row["node_name"] . "</td>";
            echo "<td>" . $row["manufacturer"] . "</td>";
            echo "<td>" . $row["longitude"] . "</td>";
            echo "<td>" . $row["latitude"] . "</td>";
            
            echo "</tr>";
            
        }
        ?>
    </table>
    <?php
    require_once('graph.html');
    ?>
    <div id="Sensor_Data">
        <canvas id="Sensor_Data"></canvas>
    </div>
    <script>
        fetchData();
    </script>
    <p>ReferenceL node_name=node-4&time_received=10-5-2022 13:30:00&temperature=55&humidity=55</p>
</body>
</html>
