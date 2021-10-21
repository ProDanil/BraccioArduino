#ifndef Controller1_hpp
#define Controller1_hpp

struct ControllerX1 {
    enum struct AngleBase {
        A0 = 0,
        A90 = 90,
        A120 = 120
    };
    enum struct AngleShoulder {
        A65 = 65,
        A80 = 80,
        A100 = 100,
        A115 = 115,
        A140 = 140
    };
    enum struct AngleElbow {
        A0 = 0,
        A180 = 180
    };
    enum struct AngleWristVer {
        A10 = 10,
        A15 = 15,
        A165 = 165,
        A170 = 170
    };
    enum struct AngleWristRot {
        A0 = 0,
        A90 = 90
    };
    enum struct AngleGripper {
        A15 = 15,
        A73 = 73
    };

    enum State {
        BEGIN,
        GO_WAIT,
        WAIT,
        GO_UP_Z0,
        GO_LOW_Z0,
        GO_PICKUP_Z0,
        GO_UP_CLENCHED_Z0,
        WAIT_ACK,
        GO_UP_CLENCHED_Z1,
        GO_LOW_CLENCHED_Z1,
        GO_DROP_Z1,
        GO_UP_Z1,
        WAIT_RELEASE,
        __SAME__
    } state = BEGIN;

    struct Input {
        bool want_cargo_on_out;
        bool is_acquired;
        bool is_done_m1;
        bool is_done_m2;
        bool is_done_m3;
        bool is_done_m4;
        bool is_done_m5;
        bool is_done_m6;
    } input;

    struct Out {
        /* TODO: initial values */
        AngleBase go_base;
        AngleShoulder go_shoulder;
        AngleElbow go_elbow;
        AngleWristVer go_wrist_ver;
        AngleWristRot go_wrist_rot;
        AngleGripper go_gripper;
        bool want_to_release;
        bool want_to_acquire;
        String st;
    } out;

    String state2string(State s) {
        switch (s) {
            case BEGIN:
                return "BEGIN";
            case GO_WAIT:
                return "GO_WAIT";
            case WAIT:
                return "WAIT";
            case GO_UP_Z0:
                return "GO_UP_Z0";
            case GO_LOW_Z0:
                return "GO_LOW_Z0";
            case GO_PICKUP_Z0:
                return "GO_PICKUP_Z0";
            case GO_UP_CLENCHED_Z0:
                return "GO_UP_CLENCHED_Z0";
            case WAIT_ACK:
                return "WAIT_ACK";
            case GO_UP_CLENCHED_Z1:
                return "GO_UP_CLENCHED_Z1";
            case GO_LOW_CLENCHED_Z1:
                return "GO_LOW_CLENCHED_Z1";
            case GO_DROP_Z1:
                return "GO_DROP_Z1";
            case GO_UP_Z1:
                return "GO_UP_Z1";
            case WAIT_RELEASE:
                return "WAIT_RELEASE";
            case __SAME__:
                return "__SAME__";
            default:
                return "";
        }
    }

    bool go_step(Input input) {
        bool is_done_all = input.is_done_m1 && input.is_done_m2 && input.is_done_m3 && input.is_done_m4 && input.is_done_m5 && input.is_done_m6;

        State next_state = __SAME__;

        // Compute next state
        if (0) {
        } else if (state == BEGIN) {
            next_state = GO_WAIT;

        } else if (state == GO_WAIT && is_done_all) {
            next_state = WAIT;

        } else if (state == WAIT && input.want_cargo_on_out) {
            next_state = GO_UP_Z0;

        } else if (state == GO_UP_Z0 && is_done_all && input.want_cargo_on_out) {
            next_state = GO_LOW_Z0;

        } else if (state == GO_LOW_Z0 && is_done_all && input.want_cargo_on_out) {
            next_state = GO_PICKUP_Z0;

        } else if (state == GO_PICKUP_Z0 && is_done_all) {
            next_state = GO_UP_CLENCHED_Z0;

        } else if (state == GO_UP_CLENCHED_Z0 && is_done_all) {
            next_state = WAIT_ACK;

        } else if (state == WAIT_ACK && input.is_acquired) {
            next_state = GO_UP_CLENCHED_Z1;

        } else if (state == GO_UP_CLENCHED_Z1 && is_done_all) {
            next_state = GO_LOW_CLENCHED_Z1;

        } else if (state == GO_LOW_CLENCHED_Z1 && is_done_all) {
            next_state = GO_DROP_Z1;

        } else if (state == GO_DROP_Z1 && is_done_all) {
            next_state = GO_UP_Z1;

        } else if (state == GO_UP_Z1 && is_done_all) {
            next_state = WAIT_RELEASE;

        } else if (((state == WAIT_RELEASE && !input.is_acquired) ||
                    state == GO_UP_Z0 || state == GO_LOW_Z0) &&
                   is_done_all) {
            next_state = GO_WAIT;
        }

        // Compute outputs
        if (0) {
        } else if (next_state == GO_WAIT) {
            out.go_base = AngleBase::A90;
            out.go_shoulder = AngleShoulder::A140;
            out.go_elbow = AngleElbow::A0;
            out.go_wrist_ver = AngleWristVer::A10;
            out.go_wrist_rot = AngleWristRot::A0;
            out.go_gripper = AngleGripper::A15;
            //out.want_to_release = false;
        } else if (next_state == WAIT) {
            out.want_to_release = false;
        } else if (next_state == GO_UP_Z0) {
            out.go_base = AngleBase::A90;
            out.go_shoulder = AngleShoulder::A115;
            out.go_elbow = AngleElbow::A0;
            out.go_wrist_ver = AngleWristVer::A15;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A15;
        } else if (next_state == GO_LOW_Z0) {
            out.go_base = AngleBase::A90;
            out.go_shoulder = AngleShoulder::A100;
            out.go_elbow = AngleElbow::A0;
            out.go_wrist_ver = AngleWristVer::A10;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A15;
        } else if (next_state == GO_PICKUP_Z0) {
            out.go_base = AngleBase::A90;
            out.go_shoulder = AngleShoulder::A100;
            out.go_elbow = AngleElbow::A0;
            out.go_wrist_ver = AngleWristVer::A10;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A73;
        } else if (next_state == GO_UP_CLENCHED_Z0) {
            out.go_base = AngleBase::A90;
            out.go_shoulder = AngleShoulder::A115;
            out.go_elbow = AngleElbow::A0;
            out.go_wrist_ver = AngleWristVer::A15;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A73;
        } else if (next_state == WAIT_ACK) {
            out.want_to_acquire = true;
        } else if (next_state == GO_UP_CLENCHED_Z1) {
            out.go_base = AngleBase::A120;
            out.go_shoulder = AngleShoulder::A65;
            out.go_elbow = AngleElbow::A180;
            out.go_wrist_ver = AngleWristVer::A165;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A73;
            out.want_to_acquire = false;
        } else if (next_state == GO_LOW_CLENCHED_Z1) {
            out.go_base = AngleBase::A120;
            out.go_shoulder = AngleShoulder::A80;
            out.go_elbow = AngleElbow::A180;
            out.go_wrist_ver = AngleWristVer::A170;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A73;
        } else if (next_state == GO_DROP_Z1) {
            out.go_base = AngleBase::A120;
            out.go_shoulder = AngleShoulder::A80;
            out.go_elbow = AngleElbow::A180;
            out.go_wrist_ver = AngleWristVer::A170;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A15;
        } else if (next_state == GO_UP_Z1) {
            out.go_base = AngleBase::A120;
            out.go_shoulder = AngleShoulder::A65;
            out.go_elbow = AngleElbow::A180;
            out.go_wrist_ver = AngleWristVer::A165;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A15;
        } else if (next_state == WAIT_RELEASE) {
            out.want_to_release = true;
        } else {
            // do nothing
        }

        // Update the state
        if (next_state != __SAME__) {
            state = next_state;
        }

        // return is_active
        return (next_state != __SAME__);
    }
};

#endif
