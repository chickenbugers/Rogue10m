"""Execute a project Python script in the currently running Unreal Editor."""

from pathlib import Path
import sys
import time


ENGINE_PYTHON = Path(
    r"D:\Program Files\UE_5.8\Engine\Plugins\Experimental"
    r"\PythonScriptPlugin\Content\Python"
)


def main():
    if len(sys.argv) != 2:
        raise RuntimeError("Usage: RunRemoteEditorScript.py <script-path>")

    target_script = Path(sys.argv[1]).resolve()
    if not target_script.is_file():
        raise RuntimeError(f"Target script does not exist: {target_script}")

    sys.path.insert(0, str(ENGINE_PYTHON))
    import remote_execution

    session = remote_execution.RemoteExecution()
    session.start()
    try:
        deadline = time.time() + 10.0
        nodes = []
        while time.time() < deadline:
            nodes = session.remote_nodes
            if nodes:
                break
            time.sleep(0.25)
        if not nodes:
            raise RuntimeError("No Unreal Editor Python remote node discovered.")

        session.open_command_connection(nodes[0]["node_id"])
        command = (
            "import unreal; "
            "unreal.EditorLevelLibrary.editor_end_play(); "
            f"path=r'{target_script}'; "
            "code=open(path, encoding='utf-8').read(); "
            "exec(compile(code, path, 'exec'), "
            "{'__file__': path, '__name__': '__main__'})"
        )
        result = session.run_command(
            command,
            exec_mode=remote_execution.MODE_EXEC_STATEMENT,
            raise_on_failure=True,
        )
        print(result)
    finally:
        session.stop()


if __name__ == "__main__":
    main()
