var _data = raw_data;
var _params = schema;


var payload = _data.payload;

log(_data);

function hexToBytes(hex) {
    for (var bytes = [], c = 0; c < hex.length; c += 2)
        bytes.push(parseInt(hex.substr(c, 2), 16));
    return bytes;
}

var obj ={};

log("Starting");
byte_data = hexToBytes( payload );
log( byte_data );

if ((byte_data[0] == 0x01 ) || (byte_data[0] == 0x02 )){
	var temperature;

	// read sensor temperature
	temperature = byte_data[1] << 8;
	temperature = temperature + byte_data[2];
					
    temperature = temperature / 100;
  
  	log("Temperature=");
    log(temperature);
  	_params.temperature.value = temperature;

	update_schema = _params;  
  	
}