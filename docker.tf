terraform { 
  required_providers { 
    docker = { 
      source  = "kreuzwerker/docker" 
      version = "~> 3.0.1" 
    } 
  } 
} 
 
provider "docker" {} 
 
 
resource "docker_image" "mst-algorithm" { 
  name         = "enmshtuz/mst-algorithm:latest" 
  keep_locally = false 
} 
 
resource "docker_container" "mst-algorithm" { 
  image   = docker_image.mst-algorithm.image_id 
  name    = "mst-algorithm" 
  restart = "unless-stopped" 
  ports { 
    internal = 80 
    external = 8080 
  } 
}
