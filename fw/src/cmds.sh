#define cmds

alias motor_enable="i2ctransfer -y 1 w1@0x45 0x02"
alias motor_disable="i2ctransfer -y 1 w1@0x45 0x01"

#home axis
alias home_y="i2ctransfer -y 1 w2@0x45 0x06 0x37"
alias home_p="i2ctransfer -y 1 w2@0x45 0x06 0x5F"
alias home_c="i2ctransfer -y 1 w2@0x45 0x06 0x9F"

#move Y 50 steps at half max speed
alias move_step_y="i2ctransfer -y 1 w4@0x45 0x03 0x0F 0x00 0x32"
alias move_step_p="i2ctransfer -y 1 w4@0x45 0x03 0x4F 0x00 0x32"
alias move_step_c="i2ctransfer -y 1 w4@0x45 0x03 0x8F 0x00 0x00"

#move continuosly
alias accel_y="i2ctransfer -y 1 w2@0x45 0x05 0x3F"
alias accel_p="i2ctransfer -y 1 w2@0x45 0x05 0x7F"
alias accel_c="i2ctransfer -y 1 w2@0x45 0x05 0xBF"

# alias stop_y=""
# alias stop_x=""
# alias stop_c=""


#laser control
alias laser_on="i2ctransfer -y 1 w2@0x45 0x07 0x01"
alias laser_off="i2ctransfer -y 1 w2@0x45 0x07 0x00"

#shoot! both guns for 100 ms

alias shoot_l="i2ctransfer -y 1 w2@0x45 0x0A 0x14"
alias shoot_r="i2ctransfer -y 1 w2@0x45 0x0A 0x15"