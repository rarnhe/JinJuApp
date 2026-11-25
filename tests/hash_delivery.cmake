foreach(name JinJuApp.exe JinJuXaml.dll JinJuXamlUnload.exe JinJuMarquee.exe)
  file(SHA256 "${DELIVERY_DIR}/${name}" digest)
  file(WRITE "${DELIVERY_DIR}/${name}.sha256" "${digest}  ${name}\n")
endforeach()
