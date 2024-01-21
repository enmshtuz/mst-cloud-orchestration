packer {  
  required_plugins {  
    docker = {  
      version = ">= 0.0.7"  
      source  = "github.com/hashicorp/docker"  
    }  
  }  
}  
  
source "docker" "ubuntu" {  
  image  = "ubuntu:xenial"  
  commit = true  
}  
  
build {  
  name    = "mst-algorithm"  
  sources = ["source.docker.ubuntu"]  
  
  provisioner "shell" {  
    inline = [  
      "apt-get update",  
      "apt-get install -y build-essential",  
      "apt-get install -y libomp-dev",  
      "apt-get install -y gcc",  # Add GCC installation  
      "apt-get clean",  
    ]  
  }  
  
  provisioner "file" {  
    source      = "prim_mst.c"  
    destination = "./prim_mst.c"  
  }  
  
  provisioner "shell" {  
    inline = [  
      "gcc -fopenmp -o prim_mst prim_mst.c",  
    ]  
  }  
  
  post-processor "docker-tag"S {  
    repository = "mst-algorithm"  
    tag        = ["ubuntu-xenial"]  
  }  
}
