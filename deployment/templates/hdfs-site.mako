<?xml version="1.0"?>
<?xml-stylesheet type="text/xsl" href="configuration.xsl"?>
<configuration>
  <property>        
    <name>dfs.name.dir</name>
    <value>/data1/name</value>
  </property>
  <property>        
    <name>dfs.data.dir</name>
    <value>/data1/data</value>
  </property>
  <!-- HUE integration -->
  <property>        
    <name>dfs.namenode.plugins</name>
    <value>org.apache.hadoop.thriftfs.NamenodePlugin</value>
  </property>
  <property>        
    <name>dfs.datanode.plugins</name>
    <value>org.apache.hadoop.thriftfs.DatanodePlugin</value>
  </property>
  <property>        
    <name>dfs.thrift.address</name>
    <value>0.0.0.0:10090</value>
  </property>
</configuration>
