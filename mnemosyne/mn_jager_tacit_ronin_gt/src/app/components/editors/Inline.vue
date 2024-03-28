<template>
  <div class="ckeditor-wrapper">
    <div :id="prop.id" />
  </div>
</template>

<script lang="ts" setup>
import {
  ref,
  onMounted,
  watch,
  defineProps,
  defineEmits,
  onBeforeUnmount
} from "vue";

const emit = defineEmits(["onChange"]);

const prop = defineProps({
  config: {
    type: Object,
    default: () => {}
  },
  id: {
    type: String,
    default: "inline-editor"
  },
  content: {
    type: String,
    default: ""
  }
});

const inlineEditor = ref<Promise<any> | any>();
const editorContent = ref(prop.content);
const interval = ref<any>(null);
let editorInstance: any = null;
onMounted(() => {
  prepareCKEditor();
});

onBeforeUnmount(() => {
  if (editorInstance) {
    editorInstance.destroy();
    inlineEditor.value = null;
  }
});

watch(editorContent, (value: any) => {
  if (interval.value) {
    clearTimeout(interval.value);
  }

  interval.value = setTimeout(() => {
    emit("onChange", value);
  }, 500);
});

const prepareCKEditor = async () => {
  if (typeof window !== "undefined") {
    const { default: InlineEditor } = await import(
      "@ckeditor/ckeditor5-build-inline"
    );
    const editorElement: HTMLElement | null = document.querySelector(
      `#${prop.id}`
    );
    if (InlineEditor && editorElement) {
      inlineEditor.value = InlineEditor.create(
        editorElement,
        prop.config && {
          ...prop.config
        }
      )
        .then((editor: any) => {
          editorInstance = editor;
          editor.model.document.on("change:data", () => {
            if (editor) {
              editorContent.value = editor.getData();
            }
          });

          // Set default data here
          editor.setData(editorContent.value);

          return editor;
        })
        .catch((error: Error) => {
          console.error("[failed at prepareCKEditor]", { error });
        });
    }
  }
};
</script>
