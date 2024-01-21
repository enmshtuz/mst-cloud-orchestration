packer {
  required_plugins {
    amazon = {
      version = ">= 0.0.3"
      source  = "github.com/hashicorp/amazon"
    }
  }
}

variable "ami_prefix" {
  type    = string
  default = "mst-algorithm"
}

locals {
  timestamp = regex_replace(timestamp(), "[- TZ:]", "")
}

source "amazon-ebs" "ubuntu" {
  ami_name      = "${var.ami_prefix}-${local.timestamp}"
  instance_type = "t2.micro"
  region        = "us-east-1"
  source_ami    = "ami-0fc5d935ebf8bc3bc"
  ssh_username  = "ubuntu"
}

build {
  name = "mst-algorithm"
  sources = [
    "source.amazon-ebs.ubuntu",
  ]

  provisioner "shell" {
    inline = [
      "sudo apt-get update",
      "sudo apt-get install -y build-essential",
      "sudo apt-get install -y libomp-dev",
      "sudo apt-get clean",
    ]
  }

  provisioner "file" {
    source      = "prim_mst.c"
    destination = "~/prim_mst.c"
  }

  provisioner "shell" {
    inline = [
      "gcc -fopenmp -o prim_mst prim_mst.c",
    ]
  }

  post-processor "vagrant" {}
}
