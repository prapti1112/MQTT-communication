#!/usr/bin/env python3

import rospy
from rospy.timer import Rate
from std_msgs.msg import String

pub, ledState = None, "off"

def ledCallback(data):
    rospy.loginfo(f'LED is {data.data}')
    if pub is not None:
        pub.publish(data.data)
    
if __name__ == '__main__':
    rospy.loginfo("Inside led state publisher....")
    rospy.init_node('ledStatePublisher', anonymous=True)
    rospy.Subscriber("led", String, ledCallback)
    pub = rospy.Publisher('led/state', String, queue_size=10)

    rospy.spin()
        