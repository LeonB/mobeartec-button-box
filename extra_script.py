Import('env')

print("HERE")
# print(env.Dump())
FRAMEWORK_DIR = env.PioPlatform().get_package_dir("framework-arduinoteensy")
print(FRAMEWORK_DIR)
print("cores/usb_serial_hid")

# env.AddPreAction("$BUILD_DIR/firmware.elf", delete_origin_usb_desc)

# def delete_origin_usb_desc:
    # None