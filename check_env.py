from SCons.Script import Import
Import("env")
import os

# Required environment variables
REQUIRED_ENV_VARS = [
    'WIFI_SSID',
    'WIFI_PASSWORD'
]

print("@@@@@ Checking environment variables")

# Check if .env file exists
env_file_path = ".env"
if not os.path.exists(env_file_path):
    print(f"❌ Error: {env_file_path} file not found!")
    print("Please create a .env file based on wifi_config.template")
    Exit(1)

# Parse .env file
env_vars = {}
try:
    with open(env_file_path, 'r') as f:
        for line_num, line in enumerate(f, 1):
            line = line.strip()
            
            # Skip empty lines and comments
            if not line or line.startswith('#'):
                continue
            
            # Parse key=value pairs
            if '=' in line:
                key, value = line.split('=', 1)
                key = key.strip()
                value = value.strip()
                
                # Remove quotes if present
                if (value.startswith('"') and value.endswith('"')) or \
                   (value.startswith("'") and value.endswith("'")):
                    value = value[1:-1]
                
                env_vars[key] = value
            else:
                print(f"⚠️  Warning: Invalid format on line {line_num}: {line}")

except IOError as e:
    print(f"❌ Error reading {env_file_path}: {e}")
    Exit(1)

# Check if all required variables are present
missing_vars = []
for var in REQUIRED_ENV_VARS:
    if var not in env_vars:
        missing_vars.append(var)
    else:
        print(f"✓ {var} = {'*' * len(env_vars[var])}")  # Hide actual values in log

if missing_vars:
    print(f"❌ Error: Missing required environment variables: {', '.join(missing_vars)}")
    print("Please check your .env file and ensure all required variables are set")
    Exit(1)

# Add WiFi credentials as preprocessor defines
wifi_ssid = env_vars.get("WIFI_SSID")
wifi_password = env_vars.get("WIFI_PASSWORD")

print(f"📡 Adding WiFi defines:")
print(f"   WIFI_SSID = \"{wifi_ssid}\"")
print(f"   WIFI_PASSWORD = \"********\"")

# Add the defines to the build environment
# Using proper escaping for C++ string literals
env.Append(CPPDEFINES=[
    ("WIFI_SSID", '\\"' + wifi_ssid + '\\"'),
    ("WIFI_PASSWORD", '\\"' + wifi_password + '\\"'),
])

print("✓ All required environment variables are set and added to build")

# Debug: Show current CPPDEFINES
print("🔍 Current CPPDEFINES:")
for define in env.get("CPPDEFINES", []):
    print(f"   {define}")