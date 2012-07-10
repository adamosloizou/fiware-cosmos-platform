<?xml version="1.0"?>
<?xml-stylesheet type="text/xsl" href="configuration.xsl"?>
<configuration>
  <property>        
    <name>mapred.job.tracker</name>
    <value>${jobtracker}:8021</value>
  </property>
  <property>        
    <name>mapred.system.dir</name>
    <value>/hadoop/mapred/system</value>
  </property>
  <property>        
    <name>mapred.local.dir</name>
    <value>${dirs}</value>
  </property>
  <property>        
    <name>mapreduce.jobtracker.staging.root.dir</name>
    <value>/user</value>
  </property>
  <property>
    <name>mapred.reduce.tasks</name>
    <value>${reduce_tasks}</value>
  </property>
  <!-- optional settings -->
  <property>
    <name>mapred.child.java.opts</name>
    <value>-Xmx350m -Djava.library.path=${hadoopgpl_native_libs}</value>
  </property>
  <property>        
    <name>mapred.child.ulimit</name>
    <value>420m</value>
  </property>
  <property>
    <name>mapred.map.output.compression.codec</name>
    <value>com.hadoop.compression.lzo.LzoCodec</value>
  </property>
  <property>
    <name>mapred.output.compress</name>
    <value>true</value>
  </property>
  <property>
    <name>mapred.output.compression.type</name>
    <value>BLOCK</value>
  </property>
  <property>
    <name>mapred.output.compression.codec</name>
    <value>com.hadoop.compression.lzo.LzoCodec</value>
  </property>
  <!-- HUE integration -->
  <property>        
    <name>jobtracker.thrift.address</name>
    <value>${jobtracker}:9290</value>
  </property>
  <property>        
    <name>mapred.jobtracker.plugins</name>
    <value>org.apache.hadoop.thriftfs.ThriftJobTrackerPlugin</value>
  </property>
</configuration>
