# supla-device for Linux

This application can be run on a Linux platform and work as a background
service.

supla-device is an application which provides channels in Supla
(see https://www.supla.org), which can be controlled and viewed by
Supla application on mobile phone, or via web interface on
https://cloud.supla.org.

# Build

## Install dependencies

For Debian based distributions:

    sudo apt install git libssl-dev build-essential libyaml-cpp-dev cmake

## Get supla-device sources

    git clone https://github.com/SUPLA/supla-device.git

## Compilation

We use CMake to control build process.
Please adjust SUPLA_DEVICE_PATH to the actual path where supla-device repository
was cloned.

    export SUPLA_DEVICE_PATH=~/supla-device
    cd supla-device/extras/examples/linux
    mkdir build
    cd build
    cmake ..
    make -j10

It should produce `supla-device-linux` binary file. Check if it is working:

    ./supla-device-linux --version

# Usage

Currently there is no automated installation available. So please follow below
instructions.

## Working modes

supla-device may work in 3 modes:
1. "normal" - default mode, when you call `./supla-device-linux` from command
line. In this mode logs are prited to console (standard output). Application
working directory depends current working directory in console.
2. "daemon" - can be started by calling `./supla-device-linux -d`. Application
is forked and runs in background. Logs are routed to syslog
(see /var/log/syslog). Current working directory is changed to "/". Currently
supla-device doesn't create PID file.
3. "service" - can be started by calling `./supla-device-linux -s`. Logs are
routed to syslog and current working directory is changed to "/"
(as in daemon mode). However separate process isn't forked and application
runs in foreground.

## Logs, problems, bugs, help

In case of any problem, please check first logs from supla-device. By default
they are printed on a console, however in daemon/service mode those can be
found in syslog:

    tail -n 100 /var/log/syslog | grep supla-device-linux

This command will show last 100 entries in syslog and filter it for
supla-device-linux string.

You can change log level by adding -D (debug) or -V (verbose debug) to your
command line, or in supla-device.yaml config file.

If you need further help, please ask on
[forum](https://en-forum.supla.org/viewforum.php?f=7) (or use any other
language variant on forum, however majority of Supla forum users speaks
English and Polish). For Polish language, you may ask questions
[here](https://forum.supla.org/viewforum.php?f=18).

If you encounter any bug, or have some new feature proposal, please report it
either on forum (links above) or as a
[Github issue](https://github.com/SUPLA/supla-device/issues).

Feel free to contribute.

## Config file location

supla-device use supla-device.yaml file for configuration. By default it checks
"etc/supla-device.yaml" relative path. So it will look in current folder when
run in "normal" mode, however in "daemon" or "service" mode, it will look
for a config file in /etc/supla-device.yaml.

You can specify your own config file:

    ./supla-device-linux -c /path/to/your/file/supla-cfg.yml

## GUID, AUTHKEY, last_state.txt

GUID and AUTHKEY is automatically generated (if missing) and stored in location:
var/lib/supla-device/guid_auth.yaml. Directory may be modified, however file
name can't.
Please make sure that user, under which supla-device will be started, has write
access to the var/lib/supla-device location. I.e. by calling:

    sudo mkdir -p /var/lib/supla-device
    sudo chown supla_user /var/lib/supla-device

Adjust "supla_user" to your user name.

last_state.txt file contain last runtime "last state" log (similar to
"last state" log available on web interface for i.e. ESP8266 devices). It is
rewritten on each startup.

Application will fail not start when it doesn't have access to this location.

Location for those files may be modified by providing providing
`state_files_path` in you config YAML file.

## Example supla-device.yaml config file

Example config file is available in `extras/examples/linux/supla-device.yaml`.
Please copy it and adjust to your needs.
If you are not familiar with YAML format, please check some examples online
before you start. Data is case sensisive and indentation is important.

## Config file parameters

### Generic configuration

Below parameters are without indentation in YAML file.

#### Parameter `name`

Device name with which it registers to Supla Server.
Parameter is optional.

Example:

    name: My fancy device

#### Parameter `log_level`

Defines log level - it enables debug and verbose debug logs.
Parameter is optional - default log level is "INFO".
Allowed values: debug, verbose

Example:

    log_level: debug

#### Parameter `state_files_path`

Defines location where supla-device will read/write GUID, AUTHKEY and
last_state.txt.
Parameter is optional - default value is: var/lib/supla-device (relative path).
Allowed values: any valid relative or absolute path where supla-device will have
proper rights to write and read files.
Please put path in double quotes.

Example:

    state_files_path: "/home/supla_user/.supla-device"

#### Parameter `security_level`

Defines if Supla server ceritficate should be validated against root CA.
Values:
 - 0 - Supla root CA is used for validation (default) - not implemented yet
 - 2 - skip certificate validation - not recommended, however this is the only
 option for now.
Parameter is optional. Default value 0.

Example:

    security_level: 2

### Supla server connection

Below parameters should be defined under `supla` key (as in examples below).

#### Parameter `server`

Defines Supla server address.
Mandatory.

#### Parameter `port`

Defines Supla server port to which device should connect to. This application
use only TLS encrypted connection, so by default port 2016 is used in Supla
for this purpose.
Parameter is optional - default value: 2016.

#### Parameter `mail`

Defines mail address which is used for user account on Supla Server.
Mandatory.

Example:

    supla:
      server: svr12.supla.org
      port: 2016
      mail: user@my_mail_server.com

# Supla channels configuration

Channels are defined as YAML array under `channels` key. Each array element
starts with `-` and should have proper indentation.

Order of channels is important. First channel will get number 0, second 1, etc.
and those will be registered in Supla server with those numbers. In Supla
device is not allowed to change order of channels, nor to change channel type,
nor to remove channel. If you do so, device will not be able to register on
Supla server and will have "Channel conflict" error (visible in logs and in
last_state.txt file). In such case you will have to remove device from Supla
Cloud with all measurement history and configuration, and register it again.

It is ok to add channels later at the end of the list.

Currently only limited number of channels are supported. Please let us know
if you need something more.

Supported channel types:
* `VirtualRelay` - related class `Supla::Control::VirtualRelay`
* `Fronius` - related class `Supla::PV::Fronius`
* `ThermometerParsed` - related class `Supla::Sensor::ThermometerParsed`
* `ImpulseCounterParsed` - related class `Supla::Sensor::ImpulseCounterParsed`
* `ElectricityMeterParsed` - related class `Supla::Sensor::ElectricityMeterParsed`
* `BinaryParsed` - related class `Supla::Sensor::BinaryParsed`

Example channels configuration (details are exaplained later):

    channels:
      - type: VirtualRelay
        name: vr1 # optional, can be used as reference for adding actions (TBD)

      - type: VirtualRelay
        name: vr2

      - type: Fronius
        ip: 192.168.1.7
        port: 80
        device_id: 1

      - type: ThermometerParsed
        name: t1
        temperature: 0
        multiplier: 1
        parser:
          name: parser_1
          type: Simple
          refresh_time_ms: 200
        source:
          name: s1
          type: File
          file: temp.txt

      - type: ThermometerParsed
        name: t2
        temperature: 3
        multiplier: 1
        parser:
          use: parser_1

      - type: ImpulseCounterParsed
        name: i1
        counter: total_m3
        multiplier: 1000
        source:
          type: File
          file: "/var/log/wmbusmeters/meter_readings/water.json"
        parser:
          type: Json

      - type: ElectricityMeterParsed
        parser:
          type: Json
        source:
          type: File
          file: tauron.json
        phase_1:
          - voltage: voltage_at_phase_1_v
            multiplier: 1
          - fwd_act_energy: total_energy_consumption_tariff_1_kwh
            multiplier: 1
          - rvr_act_energy: total_energy_production_tariff_1_kwh
            multiplier: 1
          - power_active: current_power_consumption_kw
            multiplier: 1000
          - rvr_power_active: current_power_production_kw
            multiplier: 1000
        phase_2:
          - voltage: voltage_at_phase_2_v
            multiplier: 1
          - fwd_act_energy: total_energy_consumption_tariff_2_kwh
            multiplier: 1
          - rvr_act_energy: total_energy_production_tariff_2_kwh
            multiplier: 1
        phase_3:
          - voltage: voltage_at_phase_3_v
            multiplier: 1

      - type: ThermometerParsed
        temperature: 0
        source:
          type: Cmd
      # call sensors, take line with word "temp3", split it on ":" and return second
      # value
          command: "sensors | grep temp3 | cut -d : -f2"
        parser:
          type: Simple

      - type: BinaryParsed
        state: 1
        parser:
          use: parser_1

There are some new classes (compared to standard non-Linux supla-device) which
names end with "Parsed" word. In general, those channels use `parser` and
`source` functions to get some data from your computer and put it to that
channel.

## Parsed channel `source` parameter

`source` defines from where supla-device will get data for `parser` to parsed
channel. It should be defined as a subelement of a channel.

`source` have one common mandatory parameter `type` which defines type
of used source. There is also optional `name` parameter. If you name your
source, then it can be reused for multiple parsers.

There are two supported parser types:
1. `File` - use file as an input. File name is provided by `file` parameter and
additionally you can define `expiration_time_sec` parameter. If last modification
time of a file is older than `expiration_time_sec` then this source will be
considered as invalid. `expiration_time_sec` is by default set to 10 minutes. 
2. `Cmd` - use Linux command line as an input. Command is provided by `commonad`
field.

If source was already defined earlier and you want to reuse it, you can specify
`use` parameter with proper name of previously defined source. When `use`
parameter is used, then no other source configuration parameters are allowed.

## Parsed channel `parser` parameter

Parser takes text input from previously defined `source` and converts it to
value which can be used for a parsed channel value.

There are two parsers defined:
1. `Simple` - it takes input from source and try to convert each line of text
to a floating point number. Value from each line can be referenced later by
using line index number (index counting starts with 0). I.e. please take a look
at `t1` channel above.
2. `Json` - it takes input from source and parse it as JSON format. Values can
be referenced in parsed channel by JSON key name and each value is converted to
a floating point number. I.e. please check `i1` channel above.

Type of a parser is selected with a `type` parameter. You can provide a name for
your parser with `name` parameter (named parsers can be reused for different
channels). Additionally parsers allow to configure `refresh_time_ms` parameter
which provides period of time in ms, how often parser will try to refresh data
from source. Please keep in mind that it doesn't override refresh times which
are used in channel itself. I.e. thermometers are refreshed every 10 s, while
binary senosors are refereshed every 100 ms. Default refresh time for parser is
set to 5 s, so in that case thermomenter value will update every 10 s, and
binary sensor every 5 s. If you'll set `refresh_time_ms` to `200`, then
thermometer value will still refresh every 10 s, but binary sensor value will
update every 200 ms.

If parser was already defined earlier, you can reuse it by providing `use`
parameter with a parser name.

## Parsed channel definition

Each parser channel type defines its own parameter key for fetching data
from `parser`.

Value of that parameter depends on used `parser` type. `Simple` parser use
indexes as a key (i.e. number 0, 1, 23). `Json` parser use text keys.

Additionally most values have additional `multiplier` parameter which allows to
convert input value by multiplying it by provided `multiplier`.

All channels can have `name` parameter defined. In next versions it will be
used to define relations between channels and other elements in your application.

Below you can find specification for each parsed channel type.

### `ThermometerParsed`

Add channel with "thermometer" type.

Mandatory parameter: `temperature` - defines key/index by which data is fetched
from `parser`.
Optional parameter: `multiplier` - defines multiplier for fetched value
(you can put any floating point number).

### `ImpulseCounterParsed`

Add channel with "impulse counter" type. You can define in Supla Cloud what
function it should use (i.e. energy meter, water meter, etc.)

Mandatory parameter: `counter` - defines key/index by which data is fetched
from `parser`.
Optional parameter: `multiplier` - defines multiplier for fetched value
(you can put any floating point number).

### `BinaryParsed`

Add channel with "binary sensor" type (on/off, 0/1 values etc.)

Mandatory parameter: `state` - defines key/index by which data is fetched
from `parser`.
Optional parameter: `multiplier` - defines multiplier for fetched value
(you can put any floating point number).

Values equal to "1" (approx) are considered as on/1/true. All other values
are considered as off/0/false.

### `ElectricityMeterParsed`

Add 3-phase electricity meter channel.

All paramters are optional, however it is reasonable to provide at least one ;).

There are two "global" parameters, and few parameters associated with
specific phases.

Global parameters:
* `frequency` - defines mapping for fetching voltage frequency from parser
* `multiplier` - defines multiplier value for frequency (default unit is Hz, so
if your data source provide data in Hz, you can put `multiplier: 1` or remove
this parameter completely.

Phase specific parameters:

Phase parameters are defined under `phase_1`, `phase_2`, and `phase_3` keys as
an array (each item starts with `-`).
Each parameter allows to configure optional `multiplier`.
List of allowed parameters:
* `voltage` - voltage in V
* `current` - current in A
* `fwd_act_energy` - total forward active energy in kWh
* `rvr_act_energy` - total reverse active energy in kWh
* `fwd_react_energy` - total forward reactive energy in kVA
* `rvr_react_energy` - total reverse reactive energy in kVA
* `power_active` - forward active power in W
* `rvr_power_active` - reverse active power in W
* `power_reactive` - Reactive power in VAR
* `power_apparent` - Apparent power in VA
* `phase_angle` - Phase angle (angle between voltage and current)
* `power_factor` - Power factor

`ElectricityMeterParsed` in example config file above contain definition of
mapping that can be used with
[wmbusmters](https://github.com/weetmuts/wmbusmeters) integration with Tauron
AMIplus meter on two tariff billing. Supla doesn't provide tariffs, however
in this example phase_1 energy is used for tariff 1 data, and phase_2 energy is
used for tariff 2 data.
Additionally this AMIplus integration provides separate fields for active power
as "consumption" and "production". Those are mapped to Supla active power and
reverse active power fields and then shown as one positive or negative number
(depending on actual value).

Below example may be used for
[wmbusmters](https://github.com/weetmuts/wmbusmeters) integration for Tauron
AMIplus meter on standard single tariff:

      - type: ElectricityMeterParsed
        parser:
          type: Json
        source:
          type: File
          file: tauron.json
        phase_1:
          - voltage: voltage_at_phase_1_v
          - fwd_act_energy: total_energy_consumption_kwh
          - rvr_act_energy: total_energy_production_kwh
          - power_active: current_power_consumption_kw
            multiplier: 1000
          - rvr_power_active: current_power_production_kw
            multiplier: 1000
        phase_2:
          - voltage: voltage_at_phase_2_v
        phase_3:
          - voltage: voltage_at_phase_3_v

# Running supla-device as a service

Following example will use `systemctl` for running supla-device as a service.

First, prepare configuration file in `/etc/supla-device.yaml`.

Create directory for GUID and state files with proper access rights:

    sudo mkdir -p /var/lib/supla-device
    sudo chown supla_user_name /var/lib/supla-device

Prepare service file: `/etc/systemd/system/supla-device.service`:

    [Unit]
    Description=Supla Device
    After=network-online.target

    [Service]
    User=supla_user_name
    ExecStart=/home/supla/supla-device/extras/examples/linux/build/supla-device-linux -s

    [Install]
    WantedBy=multi-user.target

Please adjust `supla_user_name` and ExecStart path to your needs.

Then call:

    sudo systemctl enable supla-device.service
    sudo systemctl start supla-device.service

And check if it works:

    sudo systemctl status supla-device.service

Example output:

    ● supla-device.service - Supla  Device
      Loaded: loaded (/etc/systemd/system/supla-device.service; enabled; vendor preset: enabled)
      Active: active (running) since Wed 2022-05-18 14:38:52 CEST; 8s ago
    Main PID: 7944 (supla-device-li)
       Tasks: 3 (limit: 4699)
      Memory: 980.0K
     CGroup: /system.slice/supla-device.service
             └─7944 /home/supla/supla-device/extras/examples/linux/build/supla-device-linux -s

    maj 18 14:38:52 supla-dev-01 supla-device-linux[7944]: Enter normal mode
    maj 18 14:38:52 supla-dev-01 supla-device-linux[7944]: Using Supla protocol version 16
    maj 18 14:38:52 supla-dev-01 supla-device-linux[7944]: LAST STATE ADDED: SuplaDevice initialized
    maj 18 14:38:52 supla-dev-01 supla-device-linux[7944]: Current status: [5] SuplaDevice initialized
    maj 18 14:38:52 supla-dev-01 supla-device-linux[7944]: Establishing connection with: beta-cloud.supla.org (port: 2016)
    maj 18 14:38:54 supla-dev-01 supla-device-linux[7944]: Connected to Supla Server
    maj 18 14:38:54 supla-dev-01 supla-device-linux[7944]: LAST STATE ADDED: Register in progress
    maj 18 14:38:54 supla-dev-01 supla-device-linux[7944]: Current status: [10] Register in progress
    maj 18 14:38:54 supla-dev-01 supla-device-linux[7944]: LAST STATE ADDED: Registered and ready
    maj 18 14:38:54 supla-dev-01 supla-device-linux[7944]: Current status: [17] Registered and ready

